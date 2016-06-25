/* Figure out which device file or directory POINT is mounted on
   and show its disk usage.
   STATP must be the result of `stat (POINT, STATP)'.  */
static void
show_point (const char *point, const struct stat *statp)
{
  struct stat disk_stats;
  struct mount_entry *me;
  struct mount_entry const *best_match = NULL;

  /* Calculate the real absolute file name for POINT, and use that to find
     the mount point.  This avoids statting unavailable mount points,
     which can hang df.  */
  char *resolved = canonicalize_file_name (point);
  if (resolved && resolved[0] == '/')
    {
      size_t resolved_len = strlen (resolved);
      size_t best_match_len = 0;

      for (me = mount_list; me; me = me->me_next)
      if (!STREQ (me->me_type, "lofs")
          && (!best_match || best_match->me_dummy || !me->me_dummy))
        {
          size_t len = strlen (me->me_mountdir);
          if (best_match_len <= len && len <= resolved_len
              && (len == 1 /* root file system */
                  || ((len == resolved_len || resolved[len] == '/')
                      && strncmp (me->me_mountdir, resolved, len) == 0)))
            {
              best_match = me;
              best_match_len = len;
            }
        }
    }
  free (resolved);
  if (best_match
      && (stat (best_match->me_mountdir, &disk_stats) != 0
          || disk_stats.st_dev != statp->st_dev))
    best_match = NULL;

  if (! best_match)
    for (me = mount_list; me; me = me->me_next)
      {
        if (me->me_dev == (dev_t) -1)
          {
            if (stat (me->me_mountdir, &disk_stats) == 0)
              me->me_dev = disk_stats.st_dev;
            else
              {
                /* Report only I/O errors.  Other errors might be
                   caused by shadowed mount points, which means POINT
                   can't possibly be on this file system.  */
                if (errno == EIO)
                  {
                    error (0, errno, "%s", quote (me->me_mountdir));
                    exit_status = EXIT_FAILURE;
                  }

                /* So we won't try and fail repeatedly. */
                me->me_dev = (dev_t) -2;
              }
          }

        if (statp->st_dev == me->me_dev
            && !STREQ (me->me_type, "lofs")
            && (!best_match || best_match->me_dummy || !me->me_dummy))
          {
            /* Skip bogus mtab entries.  */
            if (stat (me->me_mountdir, &disk_stats) != 0
                || disk_stats.st_dev != me->me_dev)
              me->me_dev = (dev_t) -2;
            else
              best_match = me;
          }
      }

  if (best_match)
    show_dev (best_match->me_devname, best_match->me_mountdir, point,
              best_match->me_type, best_match->me_dummy, best_match->me_remote,
              NULL);
  else
    {
      /* We couldn't find the mount entry corresponding to POINT.  Go ahead and
         print as much info as we can; methods that require the device to be
         present will fail at a later point.  */

      /* Find the actual mount point.  */
      char *mp = find_mount_point (point, statp);
      if (mp)
        {
          show_dev (NULL, mp, NULL, NULL, false, false, NULL);
          free (mp);
        }
    }
}

