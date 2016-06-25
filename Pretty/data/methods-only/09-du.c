/* Recursively print the sizes of the directories (and, if selected, files)
   named in FILES, the last entry of which is NULL.
   BIT_FLAGS controls how fts works.
   Return true if successful.  */

static bool
du_files (char **files, int bit_flags)
{
  bool ok = true;

  if (*files)
    {
      FTS *fts = xfts_open (files, bit_flags, NULL);

      while (1)
        {
          FTSENT *ent;

          ent = fts_read (fts);
          if (ent == NULL)
            {
              if (errno != 0)
                {
                  error (0, errno, _("fts_read failed: %s"),
                         quotearg_colon (fts->fts_path));
                  ok = false;
                }

              /* When exiting this loop early, be careful to reset the
                 global, prev_level, used in process_file.  Otherwise, its
                 (level == prev_level - 1) assertion could fail.  */
              prev_level = 0;
              break;
            }
          FTS_CROSS_CHECK (fts);

          ok &= process_file (fts, ent);
        }

      if (fts_close (fts) != 0)
        {
          error (0, errno, _("fts_close failed"));
          ok = false;
        }
    }

  return ok;
}

