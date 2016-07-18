/* This function is called once for every file system object that fts
   encounters.  fts does a depth-first traversal.  This function knows
   that and accumulates per-directory totals based on changes in
   the depth of the current entry.  It returns true on success.  */

static bool
process_file (FTS *fts, FTSENT *ent)
{
  bool ok = true;
  struct duinfo dui;
  struct duinfo dui_to_print;
  size_t level;
  static size_t n_alloc;
  /* First element of the structure contains:
     The sum of the st_size values of all entries in the single directory
     at the corresponding level.  Although this does include the st_size
     corresponding to each subdirectory, it does not include the size of
     any file in a subdirectory. Also corresponding last modified date.
     Second element of the structure contains:
     The sum of the sizes of all entries in the hierarchy at or below the
     directory at the specified level.  */
  static struct dulevel *dulvl;

  const char *file = ent->fts_path;
  const struct stat *sb = ent->fts_statp;
  int info = ent->fts_info;

  if (info == FTS_DNR)
    {
      /* An error occurred, but the size is known, so count it.  */
      error (0, ent->fts_errno, _("cannot read directory %s"), quote (file));
      ok = false;
    }
  else if (info != FTS_DP)
    {
      bool excluded = excluded_file_name (exclude, file);
      if (! excluded)
        {
          /* Make the stat buffer *SB valid, or fail noisily.  */

          if (info == FTS_NSOK)
            {
              fts_set (fts, ent, FTS_AGAIN);
              FTSENT const *e = fts_read (fts);
              assert (e == ent);
              info = ent->fts_info;
            }

          if (info == FTS_NS || info == FTS_SLNONE)
            {
              error (0, ent->fts_errno, _("cannot access %s"), quote (file));
              return false;
            }
        }

      if (excluded
          || (! opt_count_all
              && (hash_all || (! S_ISDIR (sb->st_mode) && 1 < sb->st_nlink))
              && ! hash_ins (sb->st_ino, sb->st_dev)))
        {
          /* If ignoring a directory in preorder, skip its children.
             Ignore the next fts_read output too, as it's a postorder
             visit to the same directory.  */
          if (info == FTS_D)
            {
              fts_set (fts, ent, FTS_SKIP);
              FTSENT const *e = fts_read (fts);
              assert (e == ent);
            }

          return true;
        }

      switch (info)
        {
        case FTS_D:
          return true;

        case FTS_ERR:
          /* An error occurred, but the size is known, so count it.  */
          error (0, ent->fts_errno, "%s", quote (file));
          ok = false;
          break;

        case FTS_DC:
          if (cycle_warning_required (fts, ent))
            {
              emit_cycle_warning (file);
              return false;
            }
          return true;
        }
    }

  duinfo_set (&dui,
              (apparent_size
               ? sb->st_size
               : (uintmax_t) ST_NBLOCKS (*sb) * ST_NBLOCKSIZE),
              (time_type == time_mtime ? get_stat_mtime (sb)
               : time_type == time_atime ? get_stat_atime (sb)
               : get_stat_ctime (sb)));

  level = ent->fts_level;
  dui_to_print = dui;

  if (n_alloc == 0)
    {
      n_alloc = level + 10;
      dulvl = xcalloc (n_alloc, sizeof *dulvl);
    }
  else
    {
      if (level == prev_level)
        {
          /* This is usually the most common case.  Do nothing.  */
        }
      else if (level > prev_level)
        {
          /* Descending the hierarchy.
             Clear the accumulators for *all* levels between prev_level
             and the current one.  The depth may change dramatically,
             e.g., from 1 to 10.  */
          size_t i;

          if (n_alloc <= level)
            {
              dulvl = xnrealloc (dulvl, level, 2 * sizeof *dulvl);
              n_alloc = level * 2;
            }

          for (i = prev_level + 1; i <= level; i++)
            {
              duinfo_init (&dulvl[i].ent);
              duinfo_init (&dulvl[i].subdir);
            }
        }
      else /* level < prev_level */
        {
          /* Ascending the hierarchy.
             Process a directory only after all entries in that
             directory have been processed.  When the depth decreases,
             propagate sums from the children (prev_level) to the parent.
             Here, the current level is always one smaller than the
             previous one.  */
          assert (level == prev_level - 1);
          duinfo_add (&dui_to_print, &dulvl[prev_level].ent);
          if (!opt_separate_dirs)
            duinfo_add (&dui_to_print, &dulvl[prev_level].subdir);
          duinfo_add (&dulvl[level].subdir, &dulvl[prev_level].ent);
          duinfo_add (&dulvl[level].subdir, &dulvl[prev_level].subdir);
        }
    }

  prev_level = level;

  /* Let the size of a directory entry contribute to the total for the
     containing directory, unless --separate-dirs (-S) is specified.  */
  if (! (opt_separate_dirs && IS_DIR_TYPE (info)))
    duinfo_add (&dulvl[level].ent, &dui);

  /* Even if this directory is unreadable or we can't chdir into it,
     do let its size contribute to the total. */
  duinfo_add (&tot_dui, &dui);

  if ((IS_DIR_TYPE (info) && level <= max_depth)
      || ((opt_all && level <= max_depth) || level == 0))
    print_size (&dui_to_print, file);

  return ok;
}

