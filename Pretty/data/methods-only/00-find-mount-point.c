/* Return the root mountpoint of the file system on which FILE exists, in
   malloced storage.  FILE_STAT should be the result of stating FILE.
   Give a diagnostic and return NULL if unable to determine the mount point.
   Exit if unable to restore current working directory.  */
extern char *
find_mount_point (const char *file, const struct stat *file_stat)
{
  struct saved_cwd cwd;
  struct stat last_stat;
  char *mp = NULL;		/* The malloced mount point.  */

  if (save_cwd (&cwd) != 0)
    {
      error (0, errno, _("cannot get current directory"));
      return NULL;
    }

  if (S_ISDIR (file_stat->st_mode))
    /* FILE is a directory, so just chdir there directly.  */
    {
      last_stat = *file_stat;
      if (chdir (file) < 0)
        {
          error (0, errno, _("cannot change to directory %s"), quote (file));
          return NULL;
        }
    }
  else
    /* FILE is some other kind of file; use its directory.  */
    {
      char *xdir = dir_name (file);
      char *dir;
      ASSIGN_STRDUPA (dir, xdir);
      free (xdir);

      if (chdir (dir) < 0)
        {
          error (0, errno, _("cannot change to directory %s"), quote (dir));
          return NULL;
        }

      if (stat (".", &last_stat) < 0)
        {
          error (0, errno, _("cannot stat current directory (now %s)"),
                 quote (dir));
          goto done;
        }
    }

  /* Now walk up FILE's parents until we find another file system or /,
     chdiring as we go.  LAST_STAT holds stat information for the last place
     we visited.  */
  while (true)
    {
      struct stat st;
      if (stat ("..", &st) < 0)
        {
          error (0, errno, _("cannot stat %s"), quote (".."));
          goto done;
        }
      if (st.st_dev != last_stat.st_dev || st.st_ino == last_stat.st_ino)
        /* cwd is the mount point.  */
        break;
      if (chdir ("..") < 0)
        {
          error (0, errno, _("cannot change to directory %s"), quote (".."));
          goto done;
        }
      last_stat = st;
    }

  /* Finally reached a mount point, see what it's called.  */
  mp = xgetcwd ();

done:
  /* Restore the original cwd.  */
  {
    int save_errno = errno;
    if (restore_cwd (&cwd) != 0)
      error (EXIT_FAILURE, errno,
             _("failed to return to initial working directory"));
    free_cwd (&cwd);
    errno = save_errno;
  }

  return mp;
}
