static bool
wc_file (char const *file, struct fstatus *fstatus)
{
  if (! file || STREQ (file, "-"))
    {
      have_read_stdin = true;
      if (O_BINARY && ! isatty (STDIN_FILENO))
        xfreopen (NULL, "rb", stdin);
      return wc (STDIN_FILENO, file, fstatus);
    }
  else
    {
      int fd = open (file, O_RDONLY | O_BINARY);
      if (fd == -1)
        {
          error (0, errno, "%s", file);
          return false;
        }
      else
        {
          bool ok = wc (fd, file, fstatus);
          if (close (fd) != 0)
            {
              error (0, errno, "%s", file);
              return false;
            }
          return ok;
        }
    }
}

