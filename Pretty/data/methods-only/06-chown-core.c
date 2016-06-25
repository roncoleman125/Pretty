/* Change the owner and/or group of the file specified by FTS and ENT
   to UID and/or GID as appropriate.
   If REQUIRED_UID is not -1, then skip files with any other user ID.
   If REQUIRED_GID is not -1, then skip files with any other group ID.
   CHOPT specifies additional options.
   Return true if successful.  */
static bool
change_file_owner (FTS *fts, FTSENT *ent,
                   uid_t uid, gid_t gid,
                   uid_t required_uid, gid_t required_gid,
                   struct Chown_option const *chopt)
{
  char const *file_full_name = ent->fts_path;
  char const *file = ent->fts_accpath;
  struct stat const *file_stats;
  struct stat stat_buf;
  bool ok = true;
  bool do_chown;
  bool symlink_changed = true;

  switch (ent->fts_info)
    {
    case FTS_D:
      if (chopt->recurse)
        {
          if (ROOT_DEV_INO_CHECK (chopt->root_dev_ino, ent->fts_statp))
            {
              /* This happens e.g., with "chown -R --preserve-root 0 /"
                 and with "chown -RH --preserve-root 0 symlink-to-root".  */
              ROOT_DEV_INO_WARN (file_full_name);
              /* Tell fts not to traverse into this hierarchy.  */
              fts_set (fts, ent, FTS_SKIP);
              /* Ensure that we do not process "/" on the second visit.  */
              ignore_value (fts_read (fts));
              return false;
            }
          return true;
        }
      break;

    case FTS_DP:
      if (! chopt->recurse)
        return true;
      break;

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
      if (! chopt->force_silent)
        error (0, ent->fts_errno, _("cannot access %s"),
               quote (file_full_name));
      ok = false;
      break;

    case FTS_ERR:
      if (! chopt->force_silent)
        error (0, ent->fts_errno, "%s", quote (file_full_name));
      ok = false;
      break;

    case FTS_DNR:
      if (! chopt->force_silent)
        error (0, ent->fts_errno, _("cannot read directory %s"),
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

  if (!ok)
    {
      do_chown = false;
      file_stats = NULL;
    }
  else if (required_uid == (uid_t) -1 && required_gid == (gid_t) -1
           && chopt->verbosity == V_off
           && ! chopt->root_dev_ino
           && ! chopt->affect_symlink_referent)
    {
      do_chown = true;
      file_stats = ent->fts_statp;
    }
  else
    {
      file_stats = ent->fts_statp;

      /* If this is a symlink and we're dereferencing them,
         stat it to get info on the referent.  */
      if (chopt->affect_symlink_referent && S_ISLNK (file_stats->st_mode))
        {
          if (fstatat (fts->fts_cwd_fd, file, &stat_buf, 0) != 0)
            {
              if (! chopt->force_silent)
                error (0, errno, _("cannot dereference %s"),
                       quote (file_full_name));
              ok = false;
            }

          file_stats = &stat_buf;
        }

      do_chown = (ok
                  && (required_uid == (uid_t) -1
                      || required_uid == file_stats->st_uid)
                  && (required_gid == (gid_t) -1
                      || required_gid == file_stats->st_gid));
    }

  /* This happens when chown -LR --preserve-root encounters a symlink-to-/.  */
  if (ok
      && FTSENT_IS_DIRECTORY (ent)
      && ROOT_DEV_INO_CHECK (chopt->root_dev_ino, file_stats))
    {
      ROOT_DEV_INO_WARN (file_full_name);
      return false;
    }

  if (do_chown)
    {
      if ( ! chopt->affect_symlink_referent)
        {
          ok = (lchownat (fts->fts_cwd_fd, file, uid, gid) == 0);

          /* Ignore any error due to lack of support; POSIX requires
             this behavior for top-level symbolic links with -h, and
             implies that it's required for all symbolic links.  */
          if (!ok && errno == EOPNOTSUPP)
            {
              ok = true;
              symlink_changed = false;
            }
        }
      else
        {
          /* If possible, avoid a race condition with --from=O:G and without the
             (-h) --no-dereference option.  If fts's stat call determined
             that the uid/gid of FILE matched the --from=O:G-selected
             owner and group IDs, blindly using chown(2) here could lead
             chown(1) or chgrp(1) mistakenly to dereference a *symlink*
             to an arbitrary file that an attacker had moved into the
             place of FILE during the window between the stat and
             chown(2) calls.  If FILE is a regular file or a directory
             that can be opened, this race condition can be avoided safely.  */

          enum RCH_status err
            = restricted_chown (fts->fts_cwd_fd, file, file_stats, uid, gid,
                                required_uid, required_gid);
          switch (err)
            {
            case RC_ok:
              break;

            case RC_do_ordinary_chown:
              ok = (chownat (fts->fts_cwd_fd, file, uid, gid) == 0);
              break;

            case RC_error:
              ok = false;
              break;

            case RC_inode_changed:
              /* FIXME: give a diagnostic in this case?  */
            case RC_excluded:
              do_chown = false;
              ok = false;
              break;

            default:
              abort ();
            }
        }

      /* On some systems (e.g., GNU/Linux 2.4.x),
         the chown function resets the `special' permission bits.
         Do *not* restore those bits;  doing so would open a window in
         which a malicious user, M, could subvert a chown command run
         by some other user and operating on files in a directory
         where M has write access.  */

      if (do_chown && !ok && ! chopt->force_silent)
        error (0, errno, (uid != (uid_t) -1
                          ? _("changing ownership of %s")
                          : _("changing group of %s")),
               quote (file_full_name));
    }

  if (chopt->verbosity != V_off)
    {
      bool changed =
        ((do_chown && ok && symlink_changed)
         && ! ((uid == (uid_t) -1 || uid == file_stats->st_uid)
               && (gid == (gid_t) -1 || gid == file_stats->st_gid)));

      if (changed || chopt->verbosity == V_high)
        {
          enum Change_status ch_status =
            (!ok ? CH_FAILED
             : !symlink_changed ? CH_NOT_APPLIED
             : !changed ? CH_NO_CHANGE_REQUESTED
             : CH_SUCCEEDED);
          describe_change (file_full_name, ch_status,
                           chopt->user_name, chopt->group_name);
        }
    }

  if ( ! chopt->recurse)
    fts_set (fts, ent, FTS_SKIP);

  return ok;
}

