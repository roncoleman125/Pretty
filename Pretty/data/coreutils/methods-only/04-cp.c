/* Scan the arguments, and copy each by calling copy.
   Return true if successful.  */

static bool
do_copy (int n_files, char **file, const char *target_directory,
         bool no_target_directory, struct cp_options *x)
{
  struct stat sb;
  bool new_dst = false;
  bool ok = true;

  if (n_files <= !target_directory)
    {
      if (n_files <= 0)
        error (0, 0, _("missing file operand"));
      else
        error (0, 0, _("missing destination file operand after %s"),
               quote (file[0]));
      usage (EXIT_FAILURE);
    }

  if (no_target_directory)
    {
      if (target_directory)
        error (EXIT_FAILURE, 0,
               _("cannot combine --target-directory (-t) "
                 "and --no-target-directory (-T)"));
      if (2 < n_files)
        {
          error (0, 0, _("extra operand %s"), quote (file[2]));
          usage (EXIT_FAILURE);
        }
      /* Update NEW_DST and SB, which may be checked below.  */
      ignore_value (target_directory_operand (file[n_files -1], &sb, &new_dst));
    }
  else if (!target_directory)
    {
      if (2 <= n_files
          && target_directory_operand (file[n_files - 1], &sb, &new_dst))
        target_directory = file[--n_files];
      else if (2 < n_files)
        error (EXIT_FAILURE, 0, _("target %s is not a directory"),
               quote (file[n_files - 1]));
    }

  if (target_directory)
    {
      /* cp file1...filen edir
         Copy the files `file1' through `filen'
         to the existing directory `edir'. */
      int i;

      /* Initialize these hash tables only if we'll need them.
         The problems they're used to detect can arise only if
         there are two or more files to copy.  */
      if (2 <= n_files)
        {
          dest_info_init (x);
          src_info_init (x);
        }

      for (i = 0; i < n_files; i++)
        {
          char *dst_name;
          bool parent_exists = true;  /* True if dir_name (dst_name) exists. */
          struct dir_attr *attr_list;
          char *arg_in_concat = NULL;
          char *arg = file[i];

          /* Trailing slashes are meaningful (i.e., maybe worth preserving)
             only in the source file names.  */
          if (remove_trailing_slashes)
            strip_trailing_slashes (arg);

          if (parents_option)
            {
              char *arg_no_trailing_slash;

              /* Use `arg' without trailing slashes in constructing destination
                 file names.  Otherwise, we can end up trying to create a
                 directory via `mkdir ("dst/foo/"...', which is not portable.
                 It fails, due to the trailing slash, on at least
                 NetBSD 1.[34] systems.  */
              ASSIGN_STRDUPA (arg_no_trailing_slash, arg);
              strip_trailing_slashes (arg_no_trailing_slash);

              /* Append all of `arg' (minus any trailing slash) to `dest'.  */
              dst_name = file_name_concat (target_directory,
                                           arg_no_trailing_slash,
                                           &arg_in_concat);

              /* For --parents, we have to make sure that the directory
                 dir_name (dst_name) exists.  We may have to create a few
                 leading directories. */
              parent_exists =
                (make_dir_parents_private
                 (dst_name, arg_in_concat - dst_name,
                  (x->verbose ? "%s -> %s\n" : NULL),
                  &attr_list, &new_dst, x));
            }
          else
            {
              char *arg_base;
              /* Append the last component of `arg' to `target_directory'.  */

              ASSIGN_BASENAME_STRDUPA (arg_base, arg);
              /* For `cp -R source/.. dest', don't copy into `dest/..'. */
              dst_name = (STREQ (arg_base, "..")
                          ? xstrdup (target_directory)
                          : file_name_concat (target_directory, arg_base,
                                              NULL));
            }

          if (!parent_exists)
            {
              /* make_dir_parents_private failed, so don't even
                 attempt the copy.  */
              ok = false;
            }
          else
            {
              bool copy_into_self;
              ok &= copy (arg, dst_name, new_dst, x, &copy_into_self, NULL);

              if (parents_option)
                ok &= re_protect (dst_name, arg_in_concat - dst_name,
                                  attr_list, x);
            }

          if (parents_option)
            {
              while (attr_list)
                {
                  struct dir_attr *p = attr_list;
                  attr_list = attr_list->next;
                  free (p);
                }
            }

          free (dst_name);
        }
    }
  else /* !target_directory */
    {
      char const *new_dest;
      char const *source = file[0];
      char const *dest = file[1];
      bool unused;

      if (parents_option)
        {
          error (0, 0,
                 _("with --parents, the destination must be a directory"));
          usage (EXIT_FAILURE);
        }

      /* When the force and backup options have been specified and
         the source and destination are the same name for an existing
         regular file, convert the user's command, e.g.,
         `cp --force --backup foo foo' to `cp --force foo fooSUFFIX'
         where SUFFIX is determined by any version control options used.  */

      if (x->unlink_dest_after_failed_open
          && x->backup_type != no_backups
          && STREQ (source, dest)
          && !new_dst && S_ISREG (sb.st_mode))
        {
          static struct cp_options x_tmp;

          new_dest = find_backup_file_name (dest, x->backup_type);
          /* Set x->backup_type to `no_backups' so that the normal backup
             mechanism is not used when performing the actual copy.
             backup_type must be set to `no_backups' only *after* the above
             call to find_backup_file_name -- that function uses
             backup_type to determine the suffix it applies.  */
          x_tmp = *x;
          x_tmp.backup_type = no_backups;
          x = &x_tmp;
        }
      else
        {
          new_dest = dest;
        }

      ok = copy (source, new_dest, 0, x, &unused, NULL);
    }

  return ok;
}

