/* Update the time of file FILE according to the options given.
   Return true if successful.  */

static bool
touch (const char *file)
{
  bool ok;
  int fd = -1;
  int open_errno = 0;
  struct timespec const *t = newtime;

  if (STREQ (file, "-"))
    fd = STDOUT_FILENO;
  else if (! (no_create || no_dereference))
    {
      /* Try to open FILE, creating it if necessary.  */
      int default_permissions =
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      fd = fd_reopen (STDIN_FILENO, file,
                      O_WRONLY | O_CREAT | O_NONBLOCK | O_NOCTTY,
                      default_permissions);

      /* Don't save a copy of errno if it's EISDIR, since that would lead
         touch to give a bogus diagnostic for e.g., `touch /' (assuming
         we don't own / or have write access to it).  On Solaris 5.6,
         and probably other systems, it is EINVAL.  On SunOS4, it's EPERM.  */
      if (fd == -1 && errno != EISDIR && errno != EINVAL && errno != EPERM)
        open_errno = errno;
    }

  if (change_times != (CH_ATIME | CH_MTIME))
    {
      /* We're setting only one of the time values.  */
      if (change_times == CH_MTIME)
        newtime[0].tv_nsec = UTIME_OMIT;
      else
        {
          assert (change_times == CH_ATIME);
          newtime[1].tv_nsec = UTIME_OMIT;
        }
    }

  if (amtime_now)
    {
      /* Pass NULL to futimens so it will not fail if we have
         write access to the file, but don't own it.  */
      t = NULL;
    }

  ok = (fdutimensat (fd, AT_FDCWD, (fd == STDOUT_FILENO ? NULL : file), t,
                     (no_dereference && fd == -1) ? AT_SYMLINK_NOFOLLOW : 0)
        == 0);

  if (fd == STDIN_FILENO)
    {
      if (close (STDIN_FILENO) != 0)
        {
          error (0, errno, _("closing %s"), quote (file));
          return false;
        }
    }
  else if (fd == STDOUT_FILENO)
    {
      /* Do not diagnose "touch -c - >&-".  */
      if (!ok && errno == EBADF && no_create)
        return true;
    }

  if (!ok)
    {
      if (open_errno)
        {
          /* The wording of this diagnostic should cover at least two cases:
             - the file does not exist, but the parent directory is unwritable
             - the file exists, but it isn't writable
             I think it's not worth trying to distinguish them.  */
          error (0, open_errno, _("cannot touch %s"), quote (file));
        }
      else
        {
          if (no_create && errno == ENOENT)
            return true;
          error (0, errno, _("setting times of %s"), quote (file));
        }
      return false;
    }

  return true;
}

