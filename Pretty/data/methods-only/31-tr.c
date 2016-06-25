static size_t
plain_read (char *buf, size_t size)
{
  size_t nr = safe_read (STDIN_FILENO, buf, size);
  if (nr == SAFE_READ_ERROR)
    error (EXIT_FAILURE, errno, _("read error"));
  return nr;
}

