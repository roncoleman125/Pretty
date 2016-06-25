/* Prompt whether to remove FILENAME (ent->, if required via a combination of
   the options specified by X and/or file attributes.  If the file may
   be removed, return RM_OK.  If the user declines to remove the file,
   return RM_USER_DECLINED.  If not ignoring missing files and we
   cannot lstat FILENAME, then return RM_ERROR.

   IS_DIR is true if ENT designates a directory, false otherwise.

   Depending on MODE, ask whether to `descend into' or to `remove' the
   directory FILENAME.  MODE is ignored when FILENAME is not a directory.
   Set *IS_EMPTY_P to T_YES if FILENAME is an empty directory, and it is
   appropriate to try to remove it with rmdir (e.g. recursive mode).
   Don't even try to set *IS_EMPTY_P when MODE == PA_REMOVE_DIR.  */
static enum RM_status
prompt (FTS const *fts, FTSENT const *ent, bool is_dir,
        struct rm_options const *x, enum Prompt_action mode,
        Ternary *is_empty_p)
{
  int fd_cwd = fts->fts_cwd_fd;
  char const *full_name = ent->fts_path;
  char const *filename = ent->fts_accpath;
  if (is_empty_p)
    *is_empty_p = T_UNKNOWN;

  struct stat st;
  struct stat *sbuf = &st;
  cache_stat_init (sbuf);

  int dirent_type = is_dir ? DT_DIR : DT_UNKNOWN;
  int write_protected = 0;

  /* When nonzero, this indicates that we failed to remove a child entry,
     either because the user declined an interactive prompt, or due to
     some other failure, like permissions.  */
  if (ent->fts_number)
    return RM_USER_DECLINED;

  if (x->interactive == RMI_NEVER)
    return RM_OK;

  int wp_errno = 0;
  if (!x->ignore_missing_files
      && ((x->interactive == RMI_ALWAYS) || x->stdin_tty)
      && dirent_type != DT_LNK)
    {
      write_protected = write_protected_non_symlink (fd_cwd, filename,
                                                     full_name, sbuf);
      wp_errno = errno;
    }

  if (write_protected || x->interactive == RMI_ALWAYS)
    {
      if (0 <= write_protected && dirent_type == DT_UNKNOWN)
        {
          if (cache_fstatat (fd_cwd, filename, sbuf, AT_SYMLINK_NOFOLLOW) == 0)
            {
              if (S_ISLNK (sbuf->st_mode))
                dirent_type = DT_LNK;
              else if (S_ISDIR (sbuf->st_mode))
                dirent_type = DT_DIR;
              /* Otherwise it doesn't matter, so leave it DT_UNKNOWN.  */
            }
          else
            {
              /* This happens, e.g., with `rm '''.  */
              write_protected = -1;
              wp_errno = errno;
            }
        }

      if (0 <= write_protected)
        switch (dirent_type)
          {
          case DT_LNK:
            /* Using permissions doesn't make sense for symlinks.  */
            if (x->interactive != RMI_ALWAYS)
              return RM_OK;
            break;

          case DT_DIR:
            if (!x->recursive)
              {
                write_protected = -1;
                wp_errno = EISDIR;
              }
            break;
          }

      char const *quoted_name = quote (full_name);

      if (write_protected < 0)
        {
          error (0, wp_errno, _("cannot remove %s"), quoted_name);
          return RM_ERROR;
        }

      bool is_empty;
      if (is_empty_p)
        {
          is_empty = is_empty_dir (fd_cwd, filename);
          *is_empty_p = is_empty ? T_YES : T_NO;
        }
      else
        is_empty = false;

      /* Issue the prompt.  */
      if (dirent_type == DT_DIR
          && mode == PA_DESCEND_INTO_DIR
          && !is_empty)
        fprintf (stderr,
                 (write_protected
                  ? _("%s: descend into write-protected directory %s? ")
                  : _("%s: descend into directory %s? ")),
                 program_name, quoted_name);
      else
        {
          if (cache_fstatat (fd_cwd, filename, sbuf, AT_SYMLINK_NOFOLLOW) != 0)
            {
              error (0, errno, _("cannot remove %s"), quoted_name);
              return RM_ERROR;
            }

          fprintf (stderr,
                   (write_protected
                    /* TRANSLATORS: You may find it more convenient to
                       translate "%s: remove %s (write-protected) %s? "
                       instead.  It should avoid grammatical problems
                       with the output of file_type.  */
                    ? _("%s: remove write-protected %s %s? ")
                    : _("%s: remove %s %s? ")),
                   program_name, file_type (sbuf), quoted_name);
        }

      if (!yesno ())
        return RM_USER_DECLINED;
    }
  return RM_OK;
}

