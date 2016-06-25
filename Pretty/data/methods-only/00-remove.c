/* Like fstatat, but cache the result.  If ST->st_size is -1, the
   status has not been gotten yet.  If less than -1, fstatat failed
   with errno == ST->st_ino.  Otherwise, the status has already
   been gotten, so return 0.  */
static int
cache_fstatat (int fd, char const *file, struct stat *st, int flag)
{
  if (st->st_size == -1 && fstatat (fd, file, st, flag) != 0)
    {
      st->st_size = -2;
      st->st_ino = errno;
    }
  if (0 <= st->st_size)
    return 0;
  errno = (int) st->st_ino;
  return -1;
}

