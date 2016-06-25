/* Set the file descriptor flags for FD that correspond to the nonzero bits
   in ADD_FLAGS.  The file's name is NAME.  */

static void
set_fd_flags (int fd, int add_flags, char const *name)
{
  /* Ignore file creation flags that are no-ops on file descriptors.  */
  add_flags &= ~ (O_NOCTTY | O_NOFOLLOW);

  if (add_flags)
    {
      int old_flags = fcntl (fd, F_GETFL);
      int new_flags = old_flags | add_flags;
      bool ok = true;
      if (old_flags < 0)
        ok = false;
      else if (old_flags != new_flags)
        {
          if (new_flags & (O_DIRECTORY | O_NOLINKS))
            {
              /* NEW_FLAGS contains at least one file creation flag that
                 requires some checking of the open file descriptor.  */
              struct stat st;
              if (fstat (fd, &st) != 0)
                ok = false;
              else if ((new_flags & O_DIRECTORY) && ! S_ISDIR (st.st_mode))
                {
                  errno = ENOTDIR;
                  ok = false;
                }
              else if ((new_flags & O_NOLINKS) && 1 < st.st_nlink)
                {
                  errno = EMLINK;
                  ok = false;
                }
              new_flags &= ~ (O_DIRECTORY | O_NOLINKS);
            }

          if (ok && old_flags != new_flags
              && fcntl (fd, F_SETFL, new_flags) == -1)
            ok = false;
        }

      if (!ok)
        error (EXIT_FAILURE, errno, _("setting flags for %s"), quote (name));
    }
}

