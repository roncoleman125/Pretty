static bool
head_lines (const char *filename, int fd, uintmax_t lines_to_write)
{
  char buffer[BUFSIZ];

  while (lines_to_write)
    {
      size_t bytes_read = safe_read (fd, buffer, BUFSIZ);
      size_t bytes_to_write = 0;

      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, _("error reading %s"), quote (filename));
          return false;
        }
      if (bytes_read == 0)
        break;
      while (bytes_to_write < bytes_read)
        if (buffer[bytes_to_write++] == '\n' && --lines_to_write == 0)
          {
            off_t n_bytes_past_EOL = bytes_read - bytes_to_write;
            /* If we have read more data than that on the specified number
               of lines, try to seek back to the position we would have
               gotten to had we been reading one byte at a time.  */
            if (lseek (fd, -n_bytes_past_EOL, SEEK_CUR) < 0)
              {
                int e = errno;
                struct stat st;
                if (fstat (fd, &st) != 0 || S_ISREG (st.st_mode))
                  error (0, e, _("cannot reposition file pointer for %s"),
                         quote (filename));
              }
            break;
          }
      if (fwrite (buffer, 1, bytes_to_write, stdout) < bytes_to_write)
        error (EXIT_FAILURE, errno, _("write error"));
    }
  return true;
}

