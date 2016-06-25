/* Make a link DEST to the (usually) existing file SOURCE.
   Symbolic links to nonexistent files are allowed.
   Return true if successful.  */

static bool
do_link (const char *source, const char *dest)
{
  struct stat source_stats;
  struct stat dest_stats;
  char *dest_backup = NULL;
  bool dest_lstat_ok = false;
  bool source_is_dir = false;
  bool ok;

  if (!symbolic_link)
    {
       /* Which stat to use depends on whether linkat will follow the
          symlink.  We can't use the shorter
          (logical?stat:lstat) (source, &source_stats)
          since stat might be a function-like macro.  */
      if ((logical ? stat (source, &source_stats)
           : lstat (source, &source_stats))
          != 0)
        {
          error (0, errno, _("accessing %s"), quote (source));
          return false;
        }

      if (S_ISDIR (source_stats.st_mode))
        {
          source_is_dir = true;
          if (! hard_dir_link)
            {
              error (0, 0, _("%s: hard link not allowed for directory"),
                     quote (source));
              return false;
            }
        }
    }

  if (remove_existing_files || interactive || backup_type != no_backups)
    {
      dest_lstat_ok = (lstat (dest, &dest_stats) == 0);
      if (!dest_lstat_ok && errno != ENOENT)
        {
          error (0, errno, _("accessing %s"), quote (dest));
          return false;
        }
    }

  /* If the current target was created as a hard link to another
     source file, then refuse to unlink it.  */
  if (dest_lstat_ok
      && dest_set != NULL
      && seen_file (dest_set, dest, &dest_stats))
    {
      error (0, 0,
             _("will not overwrite just-created %s with %s"),
             quote_n (0, dest), quote_n (1, source));
      return false;
    }

  /* If --force (-f) has been specified without --backup, then before
     making a link ln must remove the destination file if it exists.
     (with --backup, it just renames any existing destination file)
     But if the source and destination are the same, don't remove
     anything and fail right here.  */
  if ((remove_existing_files
       /* Ensure that "ln --backup f f" fails here, with the
          "... same file" diagnostic, below.  Otherwise, subsequent
          code would give a misleading "file not found" diagnostic.
          This case is different than the others handled here, since
          the command in question doesn't use --force.  */
       || (!symbolic_link && backup_type != no_backups))
      && dest_lstat_ok
      /* Allow `ln -sf --backup k k' to succeed in creating the
         self-referential symlink, but don't allow the hard-linking
         equivalent: `ln -f k k' (with or without --backup) to get
         beyond this point, because the error message you'd get is
         misleading.  */
      && (backup_type == no_backups || !symbolic_link)
      && (!symbolic_link || stat (source, &source_stats) == 0)
      && SAME_INODE (source_stats, dest_stats)
      /* The following detects whether removing DEST will also remove
         SOURCE.  If the file has only one link then both are surely
         the same link.  Otherwise check whether they point to the same
         name in the same directory.  */
      && (source_stats.st_nlink == 1 || same_name (source, dest)))
    {
      error (0, 0, _("%s and %s are the same file"),
             quote_n (0, source), quote_n (1, dest));
      return false;
    }

  if (dest_lstat_ok)
    {
      if (S_ISDIR (dest_stats.st_mode))
        {
          error (0, 0, _("%s: cannot overwrite directory"), quote (dest));
          return false;
        }
      if (interactive)
        {
          fprintf (stderr, _("%s: replace %s? "), program_name, quote (dest));
          if (!yesno ())
            return true;
          remove_existing_files = true;
        }

      if (backup_type != no_backups)
        {
          dest_backup = find_backup_file_name (dest, backup_type);
          if (rename (dest, dest_backup) != 0)
            {
              int rename_errno = errno;
              free (dest_backup);
              dest_backup = NULL;
              if (rename_errno != ENOENT)
                {
                  error (0, rename_errno, _("cannot backup %s"), quote (dest));
                  return false;
                }
            }
        }
    }

  ok = ((symbolic_link ? symlink (source, dest)
         : linkat (AT_FDCWD, source, AT_FDCWD, dest,
                   logical ? AT_SYMLINK_FOLLOW : 0))
        == 0);

  /* If the attempt to create a link failed and we are removing or
     backing up destinations, unlink the destination and try again.

     On the surface, POSIX describes an algorithm that states that
     'ln -f A B' will call unlink() on B before ever attempting
     link() on A.  But strictly following this has the counterintuitive
     effect of losing the contents of B, if A does not exist.
     Fortunately, POSIX 2008 clarified that an application is free
     to fail early if it can prove that continuing onwards cannot
     succeed, so we are justified in trying link() before blindly
     removing B, thus sometimes calling link() a second time during
     a successful 'ln -f A B'.

     Try to unlink DEST even if we may have backed it up successfully.
     In some unusual cases (when DEST and DEST_BACKUP are hard-links
     that refer to the same file), rename succeeds and DEST remains.
     If we didn't remove DEST in that case, the subsequent symlink or link
     call would fail.  */

  if (!ok && errno == EEXIST && (remove_existing_files || dest_backup))
    {
      if (unlink (dest) != 0)
        {
          error (0, errno, _("cannot remove %s"), quote (dest));
          free (dest_backup);
          return false;
        }

      ok = ((symbolic_link ? symlink (source, dest)
             : linkat (AT_FDCWD, source, AT_FDCWD, dest,
                       logical ? AT_SYMLINK_FOLLOW : 0))
            == 0);
    }

  if (ok)
    {
      /* Right after creating a hard link, do this: (note dest name and
         source_stats, which are also the just-linked-destinations stats) */
      record_file (dest_set, dest, &source_stats);

      if (verbose)
        {
          if (dest_backup)
            printf ("%s ~ ", quote (dest_backup));
          printf ("%s %c> %s\n", quote_n (0, dest), (symbolic_link ? '-' : '='),
                  quote_n (1, source));
        }
    }
  else
    {
      error (0, errno,
             (symbolic_link
              ? (errno != ENAMETOOLONG && *source
                 ? _("failed to create symbolic link %s")
                 : _("failed to create symbolic link %s -> %s"))
              : (errno == EMLINK && !source_is_dir
                 ? _("failed to create hard link to %.0s%s")
                 : (errno == EDQUOT || errno == EEXIST || errno == ENOSPC
                    || errno == EROFS)
                 ? _("failed to create hard link %s")
                 : _("failed to create hard link %s => %s"))),
             quote_n (0, dest), quote_n (1, source));

      if (dest_backup)
        {
          if (rename (dest_backup, dest) != 0)
            error (0, errno, _("cannot un-backup %s"), quote (dest));
        }
    }

  free (dest_backup);
  return ok;
}

