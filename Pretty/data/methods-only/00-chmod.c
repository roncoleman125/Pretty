/* Return true if the chmodable permission bits of FILE changed.
   The old mode was OLD_MODE, but it was changed to NEW_MODE.  */

static bool
mode_changed (char const *file, mode_t old_mode, mode_t new_mode)
{
  if (new_mode & (S_ISUID | S_ISGID | S_ISVTX))
    {
      /* The new mode contains unusual bits that the call to chmod may
         have silently cleared.  Check whether they actually changed.  */

      struct stat new_stats;

      if (stat (file, &new_stats) != 0)
        {
          if (! force_silent)
            error (0, errno, _("getting new attributes of %s"), quote (file));
          return false;
        }

      new_mode = new_stats.st_mode;
    }

  return ((old_mode ^ new_mode) & CHMOD_MODE_BITS) != 0;
}

