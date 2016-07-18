/* Move SOURCE onto DEST.  Handles cross-file-system moves.
   If SOURCE is a directory, DEST must not exist.
   Return true if successful.  */

static bool
do_move (const char *source, const char *dest, const struct cp_options *x)
{
  bool copy_into_self;
  bool rename_succeeded;
  bool ok = copy (source, dest, false, x, &copy_into_self, &rename_succeeded);

  if (ok)
    {
      char const *dir_to_remove;
      if (copy_into_self)
        {
          /* In general, when copy returns with copy_into_self set, SOURCE is
             the same as, or a parent of DEST.  In this case we know it's a
             parent.  It doesn't make sense to move a directory into itself, and
             besides in some situations doing so would give highly nonintuitive
             results.  Run this `mkdir b; touch a c; mv * b' in an empty
             directory.  Here's the result of running echo `find b -print`:
             b b/a b/b b/b/a b/c.  Notice that only file `a' was copied
             into b/b.  Handle this by giving a diagnostic, removing the
             copied-into-self directory, DEST (`b/b' in the example),
             and failing.  */

          dir_to_remove = NULL;
          ok = false;
        }
      else if (rename_succeeded)
        {
          /* No need to remove anything.  SOURCE was successfully
             renamed to DEST.  Or the user declined to rename a file.  */
          dir_to_remove = NULL;
        }
      else
        {
          /* This may mean SOURCE and DEST referred to different devices.
             It may also conceivably mean that even though they referred
             to the same device, rename wasn't implemented for that device.

             E.g., (from Joel N. Weber),
             [...] there might someday be cases where you can't rename
             but you can copy where the device name is the same, especially
             on Hurd.  Consider an ftpfs with a primitive ftp server that
             supports uploading, downloading and deleting, but not renaming.

             Also, note that comparing device numbers is not a reliable
             check for `can-rename'.  Some systems can be set up so that
             files from many different physical devices all have the same
             st_dev field.  This is a feature of some NFS mounting
             configurations.

             We reach this point if SOURCE has been successfully copied
             to DEST.  Now we have to remove SOURCE.

             This function used to resort to copying only when rename
             failed and set errno to EXDEV.  */

          dir_to_remove = source;
        }

      if (dir_to_remove != NULL)
        {
          struct rm_options rm_options;
          enum RM_status status;
          char const *dir[2];

          rm_option_init (&rm_options);
          rm_options.verbose = x->verbose;
          dir[0] = dir_to_remove;
          dir[1] = NULL;

          status = rm ((void*) dir, &rm_options);
          assert (VALID_STATUS (status));
          if (status == RM_ERROR)
            ok = false;
        }
    }

  return ok;
}

