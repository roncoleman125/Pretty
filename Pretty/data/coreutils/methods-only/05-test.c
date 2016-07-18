/* Find the modification time of FILE, and stuff it into *MTIME.
   Return true if successful.  */
static bool
get_mtime (char const *filename, struct timespec *mtime)
{
  struct stat finfo;
  bool ok = (stat (filename, &finfo) == 0);
  static struct timespec const zero;
  *mtime = zero;
  if (ok)
    *mtime = get_stat_mtime (&finfo);
  return ok;
}

