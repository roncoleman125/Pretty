/* Set the timestamp of symlink, FILE, to TIMESPEC.
   If this system lacks support for that, simply return 0.  */
static inline int
utimens_symlink (char const *file, struct timespec const *timespec)
{
  int err = lutimens (file, timespec);
  /* When configuring on a system with new headers and libraries, and
     running on one with a kernel that is old enough to lack the syscall,
     utimensat fails with ENOSYS.  Ignore that.  */
  if (err && errno == ENOSYS)
    err = 0;
  return err;
}

