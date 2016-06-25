/* Read the contents of the directory SRC_NAME_IN, and recursively
   copy the contents to DST_NAME_IN.  NEW_DST is true if
   DST_NAME_IN is a directory that was created previously in the
   recursion.   SRC_SB and ANCESTORS describe SRC_NAME_IN.
   Set *COPY_INTO_SELF if SRC_NAME_IN is a parent of
   FIRST_DIR_CREATED_PER_COMMAND_LINE_ARG  FIXME
   (or the same as) DST_NAME_IN; otherwise, clear it.
   Return true if successful.  */

static bool
copy_dir (char const *src_name_in, char const *dst_name_in, bool new_dst,
          const struct stat *src_sb, struct dir_list *ancestors,
          const struct cp_options *x,
          bool *first_dir_created_per_command_line_arg,
          bool *copy_into_self)
{
  char *name_space;
  char *namep;
  struct cp_options non_command_line_options = *x;
  bool ok = true;

  name_space = savedir (src_name_in);
  if (name_space == NULL)
    {
      /* This diagnostic is a bit vague because savedir can fail in
         several different ways.  */
      error (0, errno, _("cannot access %s"), quote (src_name_in));
      return false;
    }

  /* For cp's -H option, dereference command line arguments, but do not
     dereference symlinks that are found via recursive traversal.  */
  if (x->dereference == DEREF_COMMAND_LINE_ARGUMENTS)
    non_command_line_options.dereference = DEREF_NEVER;

  namep = name_space;
  while (*namep != '\0')
    {
      bool local_copy_into_self;
      char *src_name = file_name_concat (src_name_in, namep, NULL);
      char *dst_name = file_name_concat (dst_name_in, namep, NULL);

      ok &= copy_internal (src_name, dst_name, new_dst, src_sb->st_dev,
                           ancestors, &non_command_line_options, false,
                           first_dir_created_per_command_line_arg,
                           &local_copy_into_self, NULL);
      *copy_into_self |= local_copy_into_self;

      free (dst_name);
      free (src_name);

      /* If we're copying into self, there's no point in continuing,
         and in fact, that would even infloop, now that we record only
         the first created directory per command line argument.  */
      if (local_copy_into_self)
        break;

      namep += strlen (namep) + 1;
    }
  free (name_space);
  return ok;
}

