/* Remove FILEs, honoring options specified via X.
   Return RM_OK if successful.  */
enum RM_status
rm (char *const *file, struct rm_options const *x)
{
  enum RM_status rm_status = RM_OK;

  if (*file)
    {
      int bit_flags = (FTS_CWDFD
                       | FTS_NOSTAT
                       | FTS_PHYSICAL);

      if (x->one_file_system)
        bit_flags |= FTS_XDEV;

      FTS *fts = xfts_open (file, bit_flags, NULL);

      while (1)
        {
          FTSENT *ent;

          ent = fts_read (fts);
          if (ent == NULL)
            {
              if (errno != 0)
                {
                  error (0, errno, _("fts_read failed"));
                  rm_status = RM_ERROR;
                }
              break;
            }

          enum RM_status s = rm_fts (fts, ent, x);

          assert (VALID_STATUS (s));
          UPDATE_STATUS (rm_status, s);
        }

      if (fts_close (fts) != 0)
        {
          error (0, errno, _("fts_close failed"));
          rm_status = RM_ERROR;
        }
    }

  return rm_status;
}
