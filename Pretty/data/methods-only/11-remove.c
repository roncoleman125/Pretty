/* Upon unlink failure, or when the user declines to remove ENT, mark
   each of its ancestor directories, so that we know not to prompt for
   its removal.  */
static void
mark_ancestor_dirs (FTSENT *ent)
{
  FTSENT *p;
  for (p = ent->fts_parent; FTS_ROOTLEVEL <= p->fts_level; p = p->fts_parent)
    {
      if (p->fts_number)
        break;
      p->fts_number = 1;
    }
}

