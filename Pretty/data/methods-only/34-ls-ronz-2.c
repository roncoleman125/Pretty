/* Remove any entries from CWD_FILE that are for directories,
   and queue them to be listed as directories instead.
   DIRNAME is the prefix to prepend to each dirname
   to make it correct relative to ls's working dir;
   if it is null, no prefix is needed and "." and ".." should not be ignored.
   If COMMAND_LINE_ARG is true, this directory was mentioned at the top level,
   This is desirable when processing directories recursively.  */

static void
extract_dirs_from_files (char const *dirname, bool command_line_arg)
{
  size_t i;
  size_t j;
  bool ignore_dot_and_dot_dot = (dirname != NULL);

  if (dirname && LOOP_DETECT)
    {
      /* Insert a marker entry first.  When we dequeue this marker entry,
         we'll know that DIRNAME has been processed and may be removed
         from the set of active directories.  */
      queue_directory (NULL, dirname, false);
    }

  /* Queue the directories last one first, because queueing reverses the
     order.  */
  for (i = cwd_n_used; i-- != 0; )
    {
      struct fileinfo *f = sorted_file[i];

      if (is_directory (f)
          && (! ignore_dot_and_dot_dot
              || ! basename_is_dot_or_dotdot (f->name)))
        {
          if (!dirname || f->name[0] == '/')
            queue_directory (f->name, f->linkname, command_line_arg);
          else
            {
              char *name = file_name_concat (dirname, f->name, NULL);
              queue_directory (name, f->linkname, command_line_arg);
              free (name);
            }
          if (f->filetype == arg_directory)
            free (f->name);
        }
    }

  /* Now delete the directories from the table, compacting all the remaining
     entries.  */

  for (i = 0, j = 0; i < cwd_n_used; i++)
    {
      struct fileinfo *f = sorted_file[i];
      sorted_file[j] = f;
      j += (f->filetype != arg_directory);
    }
  cwd_n_used = j;
}

