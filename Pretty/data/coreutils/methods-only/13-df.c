int
main (int argc, char **argv)
{
  struct stat *stats IF_LINT ( = 0);

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  fs_select_list = NULL;
  fs_exclude_list = NULL;
  inode_format = false;
  show_all_fs = false;
  show_listed_fs = false;
  human_output_opts = -1;
  print_type = false;
  file_systems_processed = false;
  posix_format = false;
  exit_status = EXIT_SUCCESS;
  print_grand_total = false;
  grand_fsu.fsu_blocksize = 1;

  while (true)
    {
      int oi = -1;
      int c = getopt_long (argc, argv, "aB:iF:hHklmPTt:vx:", long_options,
                           &oi);
      if (c == -1)
        break;

      switch (c)
        {
        case 'a':
          show_all_fs = true;
          break;
        case 'B':
          {
            enum strtol_error e = human_options (optarg, &human_output_opts,
                                                 &output_block_size);
            if (e != LONGINT_OK)
              xstrtol_fatal (e, oi, c, long_options, optarg);
          }
          break;
        case 'i':
          inode_format = true;
          break;
        case 'h':
          human_output_opts = human_autoscale | human_SI | human_base_1024;
          output_block_size = 1;
          break;
        case 'H':
          human_output_opts = human_autoscale | human_SI;
          output_block_size = 1;
          break;
        case 'k':
          human_output_opts = 0;
          output_block_size = 1024;
          break;
        case 'l':
          show_local_fs = true;
          break;
        case 'm': /* obsolescent */
          human_output_opts = 0;
          output_block_size = 1024 * 1024;
          break;
        case 'T':
          print_type = true;
          break;
        case 'P':
          posix_format = true;
          break;
        case SYNC_OPTION:
          require_sync = true;
          break;
        case NO_SYNC_OPTION:
          require_sync = false;
          break;

        case 'F':
          /* Accept -F as a synonym for -t for compatibility with Solaris.  */
        case 't':
          add_fs_type (optarg);
          break;

        case 'v':		/* For SysV compatibility. */
          /* ignore */
          break;
        case 'x':
          add_excluded_fs_type (optarg);
          break;

        case 'c':
          print_grand_total = true;
          break;

        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (human_output_opts == -1)
    {
      if (posix_format)
        {
          human_output_opts = 0;
          output_block_size = (getenv ("POSIXLY_CORRECT") ? 512 : 1024);
        }
      else
        human_options (getenv ("DF_BLOCK_SIZE"),
                       &human_output_opts, &output_block_size);
    }

  /* Fail if the same file system type was both selected and excluded.  */
  {
    bool match = false;
    struct fs_type_list *fs_incl;
    for (fs_incl = fs_select_list; fs_incl; fs_incl = fs_incl->fs_next)
      {
        struct fs_type_list *fs_excl;
        for (fs_excl = fs_exclude_list; fs_excl; fs_excl = fs_excl->fs_next)
          {
            if (STREQ (fs_incl->fs_name, fs_excl->fs_name))
              {
                error (0, 0,
                       _("file system type %s both selected and excluded"),
                       quote (fs_incl->fs_name));
                match = true;
                break;
              }
          }
      }
    if (match)
      exit (EXIT_FAILURE);
  }

  if (optind < argc)
    {
      int i;

      /* Open each of the given entries to make sure any corresponding
         partition is automounted.  This must be done before reading the
         file system table.  */
      stats = xnmalloc (argc - optind, sizeof *stats);
      for (i = optind; i < argc; ++i)
        {
          /* Prefer to open with O_NOCTTY and use fstat, but fall back
             on using "stat", in case the file is unreadable.  */
          int fd = open (argv[i], O_RDONLY | O_NOCTTY);
          if ((fd < 0 || fstat (fd, &stats[i - optind]))
              && stat (argv[i], &stats[i - optind]))
            {
              error (0, errno, "%s", quote (argv[i]));
              exit_status = EXIT_FAILURE;
              argv[i] = NULL;
            }
          if (0 <= fd)
            close (fd);
        }
    }

  mount_list =
    read_file_system_list ((fs_select_list != NULL
                            || fs_exclude_list != NULL
                            || print_type
                            || show_local_fs));

  if (mount_list == NULL)
    {
      /* Couldn't read the table of mounted file systems.
         Fail if df was invoked with no file name arguments;
         Otherwise, merely give a warning and proceed.  */
      int status =          (optind < argc ? 0 : EXIT_FAILURE);
      const char *warning = (optind < argc ? _("Warning: ") : "");
      error (status, errno, "%s%s", warning,
             _("cannot read table of mounted file systems"));
    }

  if (require_sync)
    sync ();

  if (optind < argc)
    {
      int i;

      /* Display explicitly requested empty file systems. */
      show_listed_fs = true;

      for (i = optind; i < argc; ++i)
        if (argv[i])
          show_entry (argv[i], &stats[i - optind]);
    }
  else
    show_all_entries ();

  if (print_grand_total)
    {
      if (inode_format)
        grand_fsu.fsu_blocks = 1;
      show_dev ("total", NULL, NULL, NULL, false, false, &grand_fsu);
    }

  if (! file_systems_processed)
    error (EXIT_FAILURE, 0, _("no file systems processed"));

  exit (exit_status);
}
