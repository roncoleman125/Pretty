static void
xwrite_stdout (char const *buffer, size_t n_bytes)
{
  if (n_bytes > 0 && fwrite (buffer, 1, n_bytes, stdout) == 0)
    error (EXIT_FAILURE, errno, _("write error"));
}

