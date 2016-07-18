/* Skip N_BYTES characters from the start of pipe FD, and print
   any extra characters that were read beyond that.
   Return 1 on error, 0 if ok, -1 if EOF.  */

static int
start_bytes (const char *pretty_filename, int fd, uintmax_t n_bytes,
             uintmax_t *read_pos)
{
  char buffer[BUFSIZ];

  while (0 < n_bytes)
    {
      size_t bytes_read = safe_read (fd, buffer, BUFSIZ);
      if (bytes_read == 0)
        return -1;
      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, _("error reading %s"), quote (pretty_filename));
          return 1;
        }
      read_pos += bytes_read;
      if (bytes_read <= n_bytes)
        n_bytes -= bytes_read;
      else
        {
          size_t n_remaining = bytes_read - n_bytes;
          if (n_remaining)
            xwrite_stdout (&buffer[n_bytes], n_remaining);
          break;
        }
    }

  return 0;
}

