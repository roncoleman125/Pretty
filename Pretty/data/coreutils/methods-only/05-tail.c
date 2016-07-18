/* Close the file with descriptor FD and name FILENAME.  */

static void
close_fd (int fd, const char *filename)
{
  if (fd != -1 && fd != STDIN_FILENO && close (fd))
    {
      error (0, errno, _("closing %s (fd=%d)"), filename, fd);
    }
}

static void
write_header (const char *pretty_filename)
{
  static bool first_file = true;

  printf ("%s==> %s <==\n", (first_file ? "" : "\n"), pretty_filename);
  first_file = false;
}

