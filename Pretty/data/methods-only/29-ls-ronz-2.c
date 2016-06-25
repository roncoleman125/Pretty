/* Add a file to the current table of files.
   Verify that the file exists, and print an error message if it does not.
   Return the number of blocks that the file occupies.  */

static uintmax_t
gobble_file (char const *name, enum filetype type, ino_t inode,
             bool command_line_arg, char const *dirname)
{
  uintmax_t blocks = 0;
  struct fileinfo *f;

  /* An inode value prior to gobble_file necessarily came from readdir,
     which is not used for command line arguments.  */
  assert (! command_line_arg || inode == NOT_AN_INODE_NUMBER);

  if (cwd_n_used == cwd_n_alloc)
    {
      cwd_file = xnrealloc (cwd_file, cwd_n_alloc, 2 * sizeof *cwd_file);
      cwd_n_alloc *= 2;
    }

  f = &cwd_file[cwd_n_used];
  memset (f, '\0', sizeof *f);
  f->stat.st_ino = inode;
  f->filetype = type;

  if (command_line_arg
      || format_needs_stat
      /* When coloring a directory (we may know the type from
         direct.d_type), we have to stat it in order to indicate
         sticky and/or other-writable attributes.  */
      || (type == directory && print_with_color)
      /* When dereferencing symlinks, the inode and type must come from
         stat, but readdir provides the inode and type of lstat.  */
      || ((print_inode || format_needs_type)
          && (type == symbolic_link || type == unknown)
          && (dereference == DEREF_ALWAYS
              || (command_line_arg && dereference != DEREF_NEVER)
              || color_symlink_as_referent || check_symlink_color))
      /* Command line dereferences are already taken care of by the above
         assertion that the inode number is not yet known.  */
      || (print_inode && inode == NOT_AN_INODE_NUMBER)
      || (format_needs_type
          && (type == unknown || command_line_arg
              /* --indicator-style=classify (aka -F)
                 requires that we stat each regular file
                 to see if it's executable.  */
              || (type == normal && (indicator_style == classify
                                     /* This is so that --color ends up
                                        highlighting files with these mode
                                        bits set even when options like -F are
                                        not specified.  Note we do a redundant
                                        stat in the very unlikely case where
                                        C_CAP is set but not the others. */
                                     || (print_with_color
                                         && (is_colored (C_EXEC)
                                             || is_colored (C_SETUID)
                                             || is_colored (C_SETGID)
                                             || is_colored (C_CAP)))
                                     )))))

    {
      /* Absolute name of this file.  */
      char *absolute_name;
      bool do_deref;
      int err;

      if (name[0] == '/' || dirname[0] == 0)
        absolute_name = (char *) name;
      else
        {
          absolute_name = alloca (strlen (name) + strlen (dirname) + 2);
          attach (absolute_name, dirname, name);
        }

      switch (dereference)
        {
        case DEREF_ALWAYS:
          err = stat (absolute_name, &f->stat);
          do_deref = true;
          break;

        case DEREF_COMMAND_LINE_ARGUMENTS:
        case DEREF_COMMAND_LINE_SYMLINK_TO_DIR:
          if (command_line_arg)
            {
              bool need_lstat;
              err = stat (absolute_name, &f->stat);
              do_deref = true;

              if (dereference == DEREF_COMMAND_LINE_ARGUMENTS)
                break;

              need_lstat = (err < 0
                            ? errno == ENOENT
                            : ! S_ISDIR (f->stat.st_mode));
              if (!need_lstat)
                break;

              /* stat failed because of ENOENT, maybe indicating a dangling
                 symlink.  Or stat succeeded, ABSOLUTE_NAME does not refer to a
                 directory, and --dereference-command-line-symlink-to-dir is
                 in effect.  Fall through so that we call lstat instead.  */
            }

        default: /* DEREF_NEVER */
          err = lstat (absolute_name, &f->stat);
          do_deref = false;
          break;
        }

      if (err != 0)
        {
          /* Failure to stat a command line argument leads to
             an exit status of 2.  For other files, stat failure
             provokes an exit status of 1.  */
          file_failure (command_line_arg,
                        _("cannot access %s"), absolute_name);
          if (command_line_arg)
            return 0;

          f->name = xstrdup (name);
          cwd_n_used++;

          return 0;
        }

      f->stat_ok = true;

      /* Note has_capability() adds around 30% runtime to `ls --color`  */
      if ((type == normal || S_ISREG (f->stat.st_mode))
          && print_with_color && is_colored (C_CAP))
        f->has_capability = has_capability (absolute_name);

      if (format == long_format || print_scontext)
        {
          bool have_selinux = false;
          bool have_acl = false;
          int attr_len = (do_deref
                          ?  getfilecon (absolute_name, &f->scontext)
                          : lgetfilecon (absolute_name, &f->scontext));
          err = (attr_len < 0);

          if (err == 0)
            have_selinux = ! STREQ ("unlabeled", f->scontext);
          else
            {
              f->scontext = UNKNOWN_SECURITY_CONTEXT;

              /* When requesting security context information, don't make
                 ls fail just because the file (even a command line argument)
                 isn't on the right type of file system.  I.e., a getfilecon
                 failure isn't in the same class as a stat failure.  */
              if (errno == ENOTSUP || errno == EOPNOTSUPP || errno == ENODATA)
                err = 0;
            }

          if (err == 0 && format == long_format)
            {
              int n = file_has_acl (absolute_name, &f->stat);
              err = (n < 0);
              have_acl = (0 < n);
            }

          f->acl_type = (!have_selinux && !have_acl
                         ? ACL_T_NONE
                         : (have_selinux && !have_acl
                            ? ACL_T_SELINUX_ONLY
                            : ACL_T_YES));
          any_has_acl |= f->acl_type != ACL_T_NONE;

          if (err)
            error (0, errno, "%s", quotearg_colon (absolute_name));
        }

      if (S_ISLNK (f->stat.st_mode)
          && (format == long_format || check_symlink_color))
        {
          char *linkname;
          struct stat linkstats;

          get_link_name (absolute_name, f, command_line_arg);
          linkname = make_link_name (absolute_name, f->linkname);

          /* Avoid following symbolic links when possible, ie, when
             they won't be traced and when no indicator is needed.  */
          if (linkname
              && (file_type <= indicator_style || check_symlink_color)
              && stat (linkname, &linkstats) == 0)
            {
              f->linkok = true;

              /* Symbolic links to directories that are mentioned on the
                 command line are automatically traced if not being
                 listed as files.  */
              if (!command_line_arg || format == long_format
                  || !S_ISDIR (linkstats.st_mode))
                {
                  /* Get the linked-to file's mode for the filetype indicator
                     in long listings.  */
                  f->linkmode = linkstats.st_mode;
                }
            }
          free (linkname);
        }

      /* When not distinguishing types of symlinks, pretend we know that
         it is stat'able, so that it will be colored as a regular symlink,
         and not as an orphan.  */
      if (S_ISLNK (f->stat.st_mode) && !check_symlink_color)
        f->linkok = true;

      if (S_ISLNK (f->stat.st_mode))
        f->filetype = symbolic_link;
      else if (S_ISDIR (f->stat.st_mode))
        {
          if (command_line_arg && !immediate_dirs)
            f->filetype = arg_directory;
          else
            f->filetype = directory;
        }
      else
        f->filetype = normal;

      blocks = ST_NBLOCKS (f->stat);
      if (format == long_format || print_block_size)
        {
          char buf[LONGEST_HUMAN_READABLE + 1];
          int len = mbswidth (human_readable (blocks, buf, human_output_opts,
                                              ST_NBLOCKSIZE, output_block_size),
                              0);
          if (block_size_width < len)
            block_size_width = len;
        }

      if (format == long_format)
        {
          if (print_owner)
            {
              int len = format_user_width (f->stat.st_uid);
              if (owner_width < len)
                owner_width = len;
            }

          if (print_group)
            {
              int len = format_group_width (f->stat.st_gid);
              if (group_width < len)
                group_width = len;
            }

          if (print_author)
            {
              int len = format_user_width (f->stat.st_author);
              if (author_width < len)
                author_width = len;
            }
        }

      if (print_scontext)
        {
          int len = strlen (f->scontext);
          if (scontext_width < len)
            scontext_width = len;
        }

      if (format == long_format)
        {
          char b[INT_BUFSIZE_BOUND (uintmax_t)];
          int b_len = strlen (umaxtostr (f->stat.st_nlink, b));
          if (nlink_width < b_len)
            nlink_width = b_len;

          if (S_ISCHR (f->stat.st_mode) || S_ISBLK (f->stat.st_mode))
            {
              char buf[INT_BUFSIZE_BOUND (uintmax_t)];
              int len = strlen (umaxtostr (major (f->stat.st_rdev), buf));
              if (major_device_number_width < len)
                major_device_number_width = len;
              len = strlen (umaxtostr (minor (f->stat.st_rdev), buf));
              if (minor_device_number_width < len)
                minor_device_number_width = len;
              len = major_device_number_width + 2 + minor_device_number_width;
              if (file_size_width < len)
                file_size_width = len;
            }
          else
            {
              char buf[LONGEST_HUMAN_READABLE + 1];
              uintmax_t size = unsigned_file_size (f->stat.st_size);
              int len = mbswidth (human_readable (size, buf, human_output_opts,
                                                  1, file_output_block_size),
                                  0);
              if (file_size_width < len)
                file_size_width = len;
            }
        }
    }

  if (print_inode)
    {
      char buf[INT_BUFSIZE_BOUND (uintmax_t)];
      int len = strlen (umaxtostr (f->stat.st_ino, buf));
      if (inode_number_width < len)
        inode_number_width = len;
    }

  f->name = xstrdup (name);
  cwd_n_used++;

  return blocks;
}

