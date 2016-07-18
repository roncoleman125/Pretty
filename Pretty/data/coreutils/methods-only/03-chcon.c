/* Recursively operate on the specified FILES (the last entry
   of which is NULL).  BIT_FLAGS controls how fts works.
   Return true if successful.  */

static bool
process_files (char **files, int bit_flags)
{
  bool ok = true;

  FTS *fts = xfts_open (files, bit_flags, NULL);

  while (1)
    {
      FTSENT *ent;

      ent = fts_read (fts);
      if (ent == NULL)
        {
          if (errno != 0)
            {
              /* FIXME: try to give a better message  */
              error (0, errno, _("fts_read failed"));
              ok = false;
            }
          break;
        }

      ok &= process_file (fts, ent);
    }

  if (fts_close (fts) != 0)
    {
      error (0, errno, _("fts_close failed"));
      ok = false;
    }

  return ok;
}

