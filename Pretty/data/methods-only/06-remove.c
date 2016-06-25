/* Return true if FILENAME is a directory (and not a symlink to a directory).
   Otherwise, including the case in which lstat fails, return false.
   *ST is FILENAME's tstatus.
   Do not modify errno.  */
static inline bool
is_dir_lstat (int fd_cwd, char const *filename, struct stat *st)
{
  int saved_errno = errno;
  bool is_dir =
    (cache_fstatat (fd_cwd, filename, st, AT_SYMLINK_NOFOLLOW) == 0
     && S_ISDIR (st->st_mode));
  errno = saved_errno;
  return is_dir;
}

