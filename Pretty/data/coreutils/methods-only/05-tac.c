/* Print FILE in reverse, copying it to a temporary
   file first if it is not seekable.
   Return true if successful.  */

static bool
tac_file (const char *filename)
{
  bool ok;
  off_t file_size;
  int fd;
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

  file_size = lseek (fd, (off_t) 0, SEEK_END);

  ok = (file_size < 0 || isatty (fd)
        ? tac_nonseekable (fd, filename)
        : tac_seekable (fd, filename));

  if (!is_stdin && close (fd) != 0)
    {
      error (0, errno, _("%s: read error"), quotearg_colon (filename));
      ok = false;
    }
  return ok;
}

