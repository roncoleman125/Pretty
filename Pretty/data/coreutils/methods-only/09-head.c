static bool
head_bytes (const char *filename, int fd, uintmax_t bytes_to_write)
{
  char buffer[BUFSIZ];
  size_t bytes_to_read = BUFSIZ;

  while (bytes_to_write)
    {
      size_t bytes_read;
      if (bytes_to_write < bytes_to_read)
        bytes_to_read = bytes_to_write;
      bytes_read = safe_read (fd, buffer, bytes_to_read);
      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, _("error reading %s"), quote (filename));
          return false;
        }
      if (bytes_read == 0)
        break;
      if (fwrite (buffer, 1, bytes_read, stdout) < bytes_read)
        error (EXIT_FAILURE, errno, _("write error"));
      bytes_to_write -= bytes_read;
    }
  return true;
}

