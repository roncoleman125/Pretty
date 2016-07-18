/* Change the owner and/or group of the FILE to UID and/or GID (safely)
   only if REQUIRED_UID and REQUIRED_GID match the owner and group IDs
   of FILE.  ORIG_ST must be the result of `stat'ing FILE.

   The `safely' part above means that we can't simply use chown(2),
   since FILE might be replaced with some other file between the time
   of the preceding stat/lstat and this chown call.  So here we open
   FILE and do everything else via the resulting file descriptor.
   We first call fstat and verify that the dev/inode match those from
   the preceding stat call, and only then, if appropriate (given the
   required_uid and required_gid constraints) do we call fchown.

   Return RC_do_ordinary_chown if we can't open FILE, or if FILE is a
   special file that might have undesirable side effects when opening.
   In this case the caller can use the less-safe ordinary chown.

   Return one of the RCH_status values.  */

static enum RCH_status
restricted_chown (int cwd_fd, char const *file,
                  struct stat const *orig_st,
                  uid_t uid, gid_t gid,
                  uid_t required_uid, gid_t required_gid)
{
  enum RCH_status status = RC_ok;
  struct stat st;
  int open_flags = O_NONBLOCK | O_NOCTTY;
  int fd;

  if (required_uid == (uid_t) -1 && required_gid == (gid_t) -1)
    return RC_do_ordinary_chown;

  if (! S_ISREG (orig_st->st_mode))
    {
      if (S_ISDIR (orig_st->st_mode))
        open_flags |= O_DIRECTORY;
      else
        return RC_do_ordinary_chown;
    }

  fd = openat (cwd_fd, file, O_RDONLY | open_flags);
  if (! (0 <= fd
         || (errno == EACCES && S_ISREG (orig_st->st_mode)
             && 0 <= (fd = openat (cwd_fd, file, O_WRONLY | open_flags)))))
    return (errno == EACCES ? RC_do_ordinary_chown : RC_error);

  if (fstat (fd, &st) != 0)
    status = RC_error;
  else if (! SAME_INODE (*orig_st, st))
    status = RC_inode_changed;
  else if ((required_uid == (uid_t) -1 || required_uid == st.st_uid)
           && (required_gid == (gid_t) -1 || required_gid == st.st_gid))
    {
      if (fchown (fd, uid, gid) == 0)
        {
          status = (close (fd) == 0
                    ? RC_ok : RC_error);
          return status;
        }
      else
        {
          status = RC_error;
        }
    }

  { /* FIXME: remove these curly braces when we assume C99.  */
    int saved_errno = errno;
    close (fd);
    errno = saved_errno;
    return status;
  }
}

