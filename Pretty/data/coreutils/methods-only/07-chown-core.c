/* Change the owner and/or group of the specified FILES.
   BIT_FLAGS specifies how to treat each symlink-to-directory
   that is encountered during a recursive traversal.
   CHOPT specifies additional options.
   If UID is not -1, then change the owner id of each file to UID.
   If GID is not -1, then change the group id of each file to GID.
   If REQUIRED_UID and/or REQUIRED_GID is not -1, then change only
   files with user ID and group ID that match the non-(-1) value(s).
   Return true if successful.  */
extern bool
chown_files (char **files, int bit_flags,
             uid_t uid, gid_t gid,
             uid_t required_uid, gid_t required_gid,
             struct Chown_option const *chopt)
{
  bool ok = true;

  /* Use lstat and stat only if they're needed.  */
  int stat_flags = ((required_uid != (uid_t) -1 || required_gid != (gid_t) -1
                     || chopt->affect_symlink_referent
                     || chopt->verbosity != V_off)
                    ? 0
                    : FTS_NOSTAT);

  FTS *fts = xfts_open (files, bit_flags | stat_flags, NULL);

  while (1)
    {
      FTSENT *ent;

      ent = fts_read (fts);
      if (ent == NULL)
        {
          if (errno != 0)
            {
              /* FIXME: try to give a better message  */
              if (! chopt->force_silent)
                error (0, errno, _("fts_read failed"));
              ok = false;
            }
          break;
        }

      ok &= change_file_owner (fts, ent, uid, gid,
                               required_uid, required_gid, chopt);
    }

  if (fts_close (fts) != 0)
    {
      error (0, errno, _("fts_close failed"));
      ok = false;
    }

  return ok;
}
