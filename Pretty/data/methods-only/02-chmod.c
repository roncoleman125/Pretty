/* Change the mode of FILE.
   Return true if successful.  This function is called
   once for every file system object that fts encounters.  */

static bool
process_file (FTS *fts, FTSENT *ent)
{
  char const *file_full_name = ent->fts_path;
  char const *file = ent->fts_accpath;
  const struct stat *file_stats = ent->fts_statp;
  mode_t old_mode IF_LINT ( = 0);
  mode_t new_mode IF_LINT ( = 0);
  bool ok = true;
  bool chmod_succeeded = false;

  switch (ent->fts_info)
    {
    case FTS_DP:
      return true;

    case FTS_NS:
      /* For a top-level file or directory, this FTS_NS (stat failed)
         indicator is determined at the time of the initial fts_open call.
         With programs like chmod, chown, and chgrp, that modify
         permissions, it is possible that the file in question is
         accessible when control reaches this point.  So, if this is
         the first time we've seen the FTS_NS for this file, tell
         fts_read to stat it "again".  */
      if (ent->fts_level == 0 && ent->fts_number == 0)
        {
          ent->fts_number = 1;
          fts_set (fts, ent, FTS_AGAIN);
          return true;
        }
      if (! force_silent)
        error (0, ent->fts_errno, _("cannot access %s"),
               quote (file_full_name));
      ok = false;
      break;

    case FTS_ERR:
      if (! force_silent)
        error (0, ent->fts_errno, "%s", quote (file_full_name));
      ok = false;
      break;

    case FTS_DNR:
      if (! force_silent)
        error (0, ent->fts_errno, _("cannot read directory %s"),
               quote (file_full_name));
      ok = false;
      break;

    case FTS_SLNONE:
      if (! force_silent)
        error (0, 0, _("cannot operate on dangling symlink %s"),
               quote (file_full_name));
      ok = false;
      break;

    case FTS_DC:		/* directory that causes cycles */
      if (cycle_warning_required (fts, ent))
        {
          emit_cycle_warning (file_full_name);
          return false;
        }
      break;

    default:
      break;
    }

  if (ok && ROOT_DEV_INO_CHECK (root_dev_ino, file_stats))
    {
      ROOT_DEV_INO_WARN (file_full_name);
      /* Tell fts not to traverse into this hierarchy.  */
      fts_set (fts, ent, FTS_SKIP);
      /* Ensure that we do not process "/" on the second visit.  */
      ignore_value (fts_read (fts));
      return false;
    }

  if (ok)
    {
      old_mode = file_stats->st_mode;
      new_mode = mode_adjust (old_mode, S_ISDIR (old_mode) != 0, umask_value,
                              change, NULL);

      if (! S_ISLNK (old_mode))
        {
          if (chmodat (fts->fts_cwd_fd, file, new_mode) == 0)
            chmod_succeeded = true;
          else
            {
              if (! force_silent)
                error (0, errno, _("changing permissions of %s"),
                       quote (file_full_name));
              ok = false;
            }
        }
    }

  if (verbosity != V_off)
    {
      bool changed = (chmod_succeeded
                      && mode_changed (file, old_mode, new_mode));

      if (changed || verbosity == V_high)
        {
          enum Change_status ch_status =
            (!ok ? CH_FAILED
             : !chmod_succeeded ? CH_NOT_APPLIED
             : !changed ? CH_NO_CHANGE_REQUESTED
             : CH_SUCCEEDED);
          describe_change (file_full_name, new_mode, ch_status);
        }
    }

  if (chmod_succeeded && diagnose_surprises)
    {
      mode_t naively_expected_mode =
        mode_adjust (old_mode, S_ISDIR (old_mode) != 0, 0, change, NULL);
      if (new_mode & ~naively_expected_mode)
        {
          char new_perms[12];
          char naively_expected_perms[12];
          strmode (new_mode, new_perms);
          strmode (naively_expected_mode, naively_expected_perms);
          new_perms[10] = naively_expected_perms[10] = '\0';
          error (0, 0,
                 _("%s: new permissions are %s, not %s"),
                 quotearg_colon (file_full_name),
                 new_perms + 1, naively_expected_perms + 1);
          ok = false;
        }
    }

  if ( ! recurse)
    fts_set (fts, ent, FTS_SKIP);

  return ok;
}

