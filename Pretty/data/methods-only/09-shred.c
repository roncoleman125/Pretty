/* A wrapper with a little more checking for fds on the command line */
static bool
wipefd (int fd, char const *qname, struct randint_source *s,
        struct Options const *flags)
{
  int fd_flags = fcntl (fd, F_GETFL);

  if (fd_flags < 0)
    {
      error (0, errno, _("%s: fcntl failed"), qname);
      return false;
    }
  if (fd_flags & O_APPEND)
    {
      error (0, 0, _("%s: cannot shred append-only file descriptor"), qname);
      return false;
    }
  return do_wipefd (fd, qname, s, flags);
}

/* --- Name-wiping code --- */

/* Characters allowed in a file name - a safe universal set.  */
static char const nameset[] =
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.";

