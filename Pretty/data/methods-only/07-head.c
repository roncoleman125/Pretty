/* Output all but the last N_LINES lines of the input stream defined by
   FD, START_POS, and END_POS.
   START_POS is the starting position of the read pointer for the file
   associated with FD (may be nonzero).
   END_POS is the file offset of EOF (one larger than offset of last byte).
   Return true upon success.
   Give a diagnostic and return false upon error.

   NOTE: this code is very similar to that of tail.c's file_lines function.
   Unfortunately, factoring out some common core looks like it'd result
   in a less efficient implementation or a messy interface.  */
static bool
elide_tail_lines_seekable (const char *pretty_filename, int fd,
                           uintmax_t n_lines,
                           off_t start_pos, off_t end_pos)
{
  char buffer[BUFSIZ];
  size_t bytes_read;
  off_t pos = end_pos;

  /* Set `bytes_read' to the size of the last, probably partial, buffer;
     0 < `bytes_read' <= `BUFSIZ'.  */
  bytes_read = (pos - start_pos) % BUFSIZ;
  if (bytes_read == 0)
    bytes_read = BUFSIZ;
  /* Make `pos' a multiple of `BUFSIZ' (0 if the file is short), so that all
     reads will be on block boundaries, which might increase efficiency.  */
  pos -= bytes_read;
  if (lseek (fd, pos, SEEK_SET) < 0)
    {
      char offset_buf[INT_BUFSIZE_BOUND (pos)];
      error (0, errno, _("%s: cannot seek to offset %s"),
             pretty_filename, offtostr (pos, offset_buf));
      return false;
    }
  bytes_read = safe_read (fd, buffer, bytes_read);
  if (bytes_read == SAFE_READ_ERROR)
    {
      error (0, errno, _("error reading %s"), quote (pretty_filename));
      return false;
    }

  /* Count the incomplete line on files that don't end with a newline.  */
  if (bytes_read && buffer[bytes_read - 1] != '\n')
    --n_lines;

  while (1)
    {
      /* Scan backward, counting the newlines in this bufferfull.  */

      size_t n = bytes_read;
      while (n)
        {
          char const *nl;
          nl = memrchr (buffer, '\n', n);
          if (nl == NULL)
            break;
          n = nl - buffer;
          if (n_lines-- == 0)
            {
              /* Found it.  */
              /* If necessary, restore the file pointer and copy
                 input to output up to position, POS.  */
              if (start_pos < pos)
                {
                  enum Copy_fd_status err;
                  if (lseek (fd, start_pos, SEEK_SET) < 0)
                    {
                      /* Failed to reposition file pointer.  */
                      error (0, errno,
                         "%s: unable to restore file pointer to initial offset",
                             quote (pretty_filename));
                      return false;
                    }

                  err = copy_fd (fd, stdout, pos - start_pos);
                  if (err != COPY_FD_OK)
                    {
                      diagnose_copy_fd_failure (err, pretty_filename);
                      return false;
                    }
                }

              /* Output the initial portion of the buffer
                 in which we found the desired newline byte.
                 Don't bother testing for failure for such a small amount.
                 Any failure will be detected upon close.  */
              fwrite (buffer, 1, n + 1, stdout);
              return true;
            }
        }

      /* Not enough newlines in that bufferfull.  */
      if (pos == start_pos)
        {
          /* Not enough lines in the file.  */
          return true;
        }
      pos -= BUFSIZ;
      if (lseek (fd, pos, SEEK_SET) < 0)
        {
          char offset_buf[INT_BUFSIZE_BOUND (pos)];
          error (0, errno, _("%s: cannot seek to offset %s"),
                 pretty_filename, offtostr (pos, offset_buf));
          return false;
        }

      bytes_read = safe_read (fd, buffer, BUFSIZ);
      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, _("error reading %s"), quote (pretty_filename));
          return false;
        }

      /* FIXME: is this dead code?
         Consider the test, pos == start_pos, above. */
      if (bytes_read == 0)
        return true;
    }
}

