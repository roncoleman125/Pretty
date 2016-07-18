/* Set the timestamps of file TO to match those of file FROM.
   Return true if successful.  */

static bool
change_timestamps (struct stat const *from_sb, char const *to)
{
  struct timespec timespec[2];
  timespec[0] = get_stat_atime (from_sb);
  timespec[1] = get_stat_mtime (from_sb);

  if (utimens (to, timespec))
    {
      error (0, errno, _("cannot set time stamps for %s"), quote (to));
      return false;
    }
  return true;
}

