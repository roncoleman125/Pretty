/* Read directory NAME, and list the files in it.
   If REALNAME is nonzero, print its name instead of NAME;
   this is used for symbolic links to directories.
   COMMAND_LINE_ARG means this directory was mentioned on the command line.  */

static void
print_dir (char const *name, char const *realname, bool command_line_arg)
{
  DIR *dirp;
  struct dirent *next;
  uintmax_t total_blocks = 0;
  static bool first = true;

  errno = 0;
  dirp = opendir (name);
  if (!dirp)
    {
      file_failure (command_line_arg, _("cannot open directory %s"), name);
      return;
    }

  if (LOOP_DETECT)
    {
      struct stat dir_stat;
      int fd = dirfd (dirp);

      /* If dirfd failed, endure the overhead of using stat.  */
      if ((0 <= fd
           ? fstat (fd, &dir_stat)
           : stat (name, &dir_stat)) < 0)
        {
          file_failure (command_line_arg,
                        _("cannot determine device and inode of %s"), name);
          closedir (dirp);
          return;
        }

      /* If we've already visited this dev/inode pair, warn that
         we've found a loop, and do not process this directory.  */
      if (visit_dir (dir_stat.st_dev, dir_stat.st_ino))
        {
          error (0, 0, _("%s: not listing already-listed directory"),
                 quotearg_colon (name));
          closedir (dirp);
          set_exit_status (true);
          return;
        }

      DEV_INO_PUSH (dir_stat.st_dev, dir_stat.st_ino);
    }

  if (recursive || print_dir_name)
    {
      if (!first)
        DIRED_PUTCHAR ('\n');
      first = false;
      DIRED_INDENT ();
      PUSH_CURRENT_DIRED_POS (&subdired_obstack);
      dired_pos += quote_name (stdout, realname ? realname : name,
                               dirname_quoting_options, NULL);
      PUSH_CURRENT_DIRED_POS (&subdired_obstack);
      DIRED_FPUTS_LITERAL (":\n", stdout);
    }

  /* Read the directory entries, and insert the subfiles into the `cwd_file'
     table.  */

  clear_files ();

  while (1)
    {
      /* Set errno to zero so we can distinguish between a readdir failure
         and when readdir simply finds that there are no more entries.  */
      errno = 0;
      next = readdir (dirp);
      if (next)
        {
          if (! file_ignored (next->d_name))
            {
              enum filetype type = unknown;

              switch (next->d_type)
                {
                case DT_BLK:  type = blockdev;		break;
                case DT_CHR:  type = chardev;		break;
                case DT_DIR:  type = directory;		break;
                case DT_FIFO: type = fifo;		break;
                case DT_LNK:  type = symbolic_link;	break;
                case DT_REG:  type = normal;		break;
                case DT_SOCK: type = sock;		break;
                case DT_WHT:  type = whiteout;		break;
                }
              total_blocks += gobble_file (next->d_name, type,
                                           RELIABLE_D_INO (next),
                                           false, name);

              /* In this narrow case, print out each name right away, so
                 ls uses constant memory while processing the entries of
                 this directory.  Useful when there are many (millions)
                 of entries in a directory.  */
              if (format == one_per_line && sort_type == sort_none
                      && !print_block_size && !recursive)
                {
                  /* We must call sort_files in spite of
                     "sort_type == sort_none" for its initialization
                     of the sorted_file vector.  */
                  sort_files ();
                  print_current_files ();
                  clear_files ();
                }
            }
        }
      else if (errno != 0)
        {
          file_failure (command_line_arg, _("reading directory %s"), name);
          if (errno != EOVERFLOW)
            break;
        }
      else
        break;
    }

  if (closedir (dirp) != 0)
    {
      file_failure (command_line_arg, _("closing directory %s"), name);
      /* Don't return; print whatever we got.  */
    }

  /* Sort the directory contents.  */
  sort_files ();

  /* If any member files are subdirectories, perhaps they should have their
     contents listed rather than being mentioned here as files.  */

  if (recursive)
    extract_dirs_from_files (name, command_line_arg);

  if (format == long_format || print_block_size)
    {
      const char *p;
      char buf[LONGEST_HUMAN_READABLE + 1];

      DIRED_INDENT ();
      p = _("total");
      DIRED_FPUTS (p, stdout, strlen (p));
      DIRED_PUTCHAR (' ');
      p = human_readable (total_blocks, buf, human_output_opts,
                          ST_NBLOCKSIZE, output_block_size);
      DIRED_FPUTS (p, stdout, strlen (p));
      DIRED_PUTCHAR ('\n');
    }

  if (cwd_n_used)
    print_current_files ();
}

