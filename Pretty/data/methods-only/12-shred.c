/*
 * Finally, the function that actually takes a filename and grinds
 * it into hamburger.
 *
 * FIXME
 * Detail to note: since we do not restore errno to EACCES after
 * a failed chmod, we end up printing the error code from the chmod.
 * This is actually the error that stopped us from proceeding, so
 * it's arguably the right one, and in practice it'll be either EACCES
 * again or EPERM, which both give similar error messages.
 * Does anyone disagree?
 */
static bool
wipefile (char *name, char const *qname,
          struct randint_source *s, struct Options const *flags)
{
  bool ok;
  int fd;

  fd = open (name, O_WRONLY | O_NOCTTY | O_BINARY);
  if (fd < 0
      && (errno == EACCES && flags->force)
      && chmod (name, S_IWUSR) == 0)
    fd = open (name, O_WRONLY | O_NOCTTY | O_BINARY);
  if (fd < 0)
    {
      error (0, errno, _("%s: failed to open for writing"), qname);
      return false;
    }

  ok = do_wipefd (fd, qname, s, flags);
  if (close (fd) != 0)
    {
      error (0, errno, _("%s: failed to close"), qname);
      ok = false;
    }
  if (ok && flags->remove_file)
    ok = wipename (name, qname, flags);
  return ok;
}


/* Buffers for random data.  */
static struct randint_source *randint_source;

