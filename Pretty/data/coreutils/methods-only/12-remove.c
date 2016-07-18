/* Remove the file system object specified by ENT.  IS_DIR specifies
   whether it is expected to be a directory or non-directory.
   Return RM_OK upon success, else RM_ERROR.  */
static enum RM_status
excise (FTS *fts, FTSENT *ent, struct rm_options const *x, bool is_dir)
{
  int flag = is_dir ? AT_REMOVEDIR : 0;
  if (unlinkat (fts->fts_cwd_fd, ent->fts_accpath, flag) == 0)
    {
      if (x->verbose)
        {
          printf ((is_dir
                   ? _("removed directory: %s\n")
                   : _("removed %s\n")), quote (ent->fts_path));
        }
      return RM_OK;
    }

  /* The unlinkat from kernels like linux-2.6.32 reports EROFS even for
     nonexistent files.  When the file is indeed missing, map that to ENOENT,
     so that rm -f ignores it, as required.  Even without -f, this is useful
     because it makes rm print the more precise diagnostic.  */
  if (errno == EROFS)
    {
      struct stat st;
      if ( ! (lstatat (fts->fts_cwd_fd, ent->fts_accpath, &st)
                       && errno == ENOENT))
        errno = EROFS;
    }

  if (ignorable_missing (x, errno))
    return RM_OK;

  /* When failing to rmdir an unreadable directory, the typical
     errno value is EISDIR, but that is not as useful to the user
     as the errno value from the failed open (probably EPERM).
     Use the earlier, more descriptive errno value.  */
  if (ent->fts_info == FTS_DNR)
    errno = ent->fts_errno;
  error (0, errno, _("cannot remove %s"), quote (ent->fts_path));
  mark_ancestor_dirs (ent);
  return RM_ERROR;
}

