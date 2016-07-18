/* If requested, delete all the files we have created.  This function
   must be called only from critical sections.  */

static void
delete_all_files (bool in_signal_handler)
{
  unsigned int i;

  if (! remove_files)
    return;

  for (i = 0; i < files_created; i++)
    {
      const char *name = make_filename (i);
      if (unlink (name) != 0 && !in_signal_handler)
        error (0, errno, "%s", name);
    }

  files_created = 0;
}

