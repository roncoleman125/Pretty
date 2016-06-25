/* Tell fts not to traverse into the hierarchy at ENT.  */
static void
fts_skip_tree (FTS *fts, FTSENT *ent)
{
  fts_set (fts, ent, FTS_SKIP);
  /* Ensure that we do not process ENT a second time.  */
  ignore_value (fts_read (fts));
}

