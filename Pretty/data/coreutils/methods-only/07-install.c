/* Copy file FROM onto file TO, creating any missing parent directories of TO.
   Return true if successful.  */

static bool
install_file_in_file_parents (char const *from, char *to,
                              struct cp_options *x)
{
  bool save_working_directory =
    ! (IS_ABSOLUTE_FILE_NAME (from) && IS_ABSOLUTE_FILE_NAME (to));
  int status = EXIT_SUCCESS;

  struct savewd wd;
  savewd_init (&wd);
  if (! save_working_directory)
    savewd_finish (&wd);

  if (mkancesdirs (to, &wd, make_ancestor, x) == -1)
    {
      error (0, errno, _("cannot create directory %s"), to);
      status = EXIT_FAILURE;
    }

  if (save_working_directory)
    {
      int restore_result = savewd_restore (&wd, status);
      int restore_errno = errno;
      savewd_finish (&wd);
      if (EXIT_SUCCESS < restore_result)
        return false;
      if (restore_result < 0 && status == EXIT_SUCCESS)
        {
          error (0, restore_errno, _("cannot create directory %s"), to);
          return false;
        }
    }

  return (status == EXIT_SUCCESS && install_file_in_file (from, to, x));
}

