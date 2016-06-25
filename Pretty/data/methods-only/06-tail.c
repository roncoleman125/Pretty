/* Read and output N_BYTES of file PRETTY_FILENAME starting at the current
   position in FD.  If N_BYTES is COPY_TO_EOF, then copy until end of file.
   If N_BYTES is COPY_A_BUFFER, then copy at most one buffer's worth.
   Return the number of bytes read from the file.  */

static uintmax_t
dump_remainder (const char *pretty_filename, int fd, uintmax_t n_bytes)
{
  uintmax_t n_written;
  uintmax_t n_remaining = n_bytes;

  n_written = 0;
  while (1)
    {
      char buffer[BUFSIZ];
      size_t n = MIN (n_remaining, BUFSIZ);
      size_t bytes_read = safe_read (fd, buffer, n);
      if (bytes_read == SAFE_READ_ERROR)
        {
          if (errno != EAGAIN)
            error (EXIT_FAILURE, errno, _("error reading %s"),
                   quote (pretty_filename));
          break;
        }
      if (bytes_read == 0)
        break;
      xwrite_stdout (buffer, bytes_read);
      n_written += bytes_read;
      if (n_bytes != COPY_TO_EOF)
        {
          n_remaining -= bytes_read;
          if (n_remaining == 0 || n_bytes == COPY_A_BUFFER)
            break;
        }
    }

  return n_written;
}

