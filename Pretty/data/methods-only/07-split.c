/* Split into pieces that are as large as possible while still not more
   than N_BYTES bytes, and are split on line boundaries except
   where lines longer than N_BYTES bytes occur.
   FIXME: Allow N_BYTES to be any uintmax_t value, and don't require a
   buffer of size N_BYTES, in case N_BYTES is very large.  */

static void
line_bytes_split (size_t n_bytes)
{
  char *bp;
  bool eof = false;
  size_t n_buffered = 0;
  char *buf = xmalloc (n_bytes);

  do
    {
      /* Fill up the full buffer size from the input file.  */

      size_t to_read = n_bytes - n_buffered;
      size_t n_read = full_read (STDIN_FILENO, buf + n_buffered, to_read);
      if (n_read < to_read && errno)
        error (EXIT_FAILURE, errno, "%s", infile);

      n_buffered += n_read;
      if (n_buffered != n_bytes)
        {
          if (n_buffered == 0)
            break;
          eof = true;
        }

      /* Find where to end this chunk.  */
      bp = buf + n_buffered;
      if (n_buffered == n_bytes)
        {
          while (bp > buf && bp[-1] != '\n')
            bp--;
        }

      /* If chunk has no newlines, use all the chunk.  */
      if (bp == buf)
        bp = buf + n_buffered;

      /* Output the chars as one output file.  */
      cwrite (true, buf, bp - buf);

      /* Discard the chars we just output; move rest of chunk
         down to be the start of the next chunk.  Source and
         destination probably overlap.  */
      n_buffered -= bp - buf;
      if (n_buffered > 0)
        memmove (buf, bp, n_buffered);
    }
  while (!eof);
  free (buf);
}

