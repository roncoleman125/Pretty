/* Remove any empty parent directories of DIR.
   If DIR contains slash characters, at least one of them
   (beginning with the rightmost) is replaced with a NUL byte.
   Return true if successful.  */

static bool
remove_parents (char *dir)
{
  char *slash;
  bool ok = true;

  strip_trailing_slashes (dir);
  while (1)
    {
      slash = strrchr (dir, '/');
      if (slash == NULL)
        break;
      /* Remove any characters after the slash, skipping any extra
         slashes in a row. */
      while (slash > dir && *slash == '/')
        --slash;
      slash[1] = 0;

      /* Give a diagnostic for each attempted removal if --verbose.  */
      if (verbose)
        prog_fprintf (stdout, _("removing directory, %s"), quote (dir));

      ok = (rmdir (dir) == 0);

      if (!ok)
        {
          /* Stop quietly if --ignore-fail-on-non-empty. */
          if (ignorable_failure (errno, dir))
            {
              ok = true;
            }
          else
            {
              /* Barring race conditions, DIR is expected to be a directory.  */
              error (0, errno, _("failed to remove directory %s"),
                     quote (dir));
            }
          break;
        }
    }
  return ok;
}

