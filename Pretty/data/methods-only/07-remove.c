/* Return true if FILENAME is a non-directory.
   Otherwise, including the case in which lstat fails, return false.
   *ST is FILENAME's tstatus.
   Do not modify errno.  */
static inline bool
is_nondir_lstat (int fd_cwd, char const *filename, struct stat *st)
{
  int saved_errno = errno;
  bool is_non_dir =
    (cache_fstatat (fd_cwd, filename, st, AT_SYMLINK_NOFOLLOW) == 0
     && !S_ISDIR (st->st_mode));
  errno = saved_errno;
  return is_non_dir;
}

