static bool
head_file (const char *filename, uintmax_t n_units, bool count_lines,
           bool elide_from_end)
{
  int fd;
  bool ok;
  bool is_stdin = STREQ (filename, "-");

  if (is_stdin)
    {
      have_read_stdin = true;
      fd = STDIN_FILENO;
      filename = _("standard input");
      if (O_BINARY && ! isatty (STDIN_FILENO))
        xfreopen (NULL, "rb", stdin);
    }
  else
    {
      fd = open (filename, O_RDONLY | O_BINARY);
      if (fd < 0)
        {
          error (0, errno, _("cannot open %s for reading"), quote (filename));
          return false;
        }
    }

  ok = head (filename, fd, n_units, count_lines, elide_from_end);
  if (!is_stdin && close (fd) != 0)
    {
      error (0, errno, _("closing %s"), quote (filename));
      return false;
    }
  return ok;
}

