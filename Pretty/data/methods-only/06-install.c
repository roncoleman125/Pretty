int
main (int argc, char **argv)
{
  int optc;
  int exit_status = EXIT_SUCCESS;
  const char *specified_mode = NULL;
  bool make_backups = false;
  char *backup_suffix_string;
  char *version_control_string = NULL;
  bool mkdir_and_install = false;
  struct cp_options x;
  char const *target_directory = NULL;
  bool no_target_directory = false;
  int n_files;
  char **file;
  bool strip_program_specified = false;
  security_context_t scontext = NULL;
  /* set iff kernel has extra selinux system calls */
  selinux_enabled = (0 < is_selinux_enabled ());

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdin);

  cp_option_init (&x);

  owner_name = NULL;
  group_name = NULL;
  strip_files = false;
  dir_arg = false;
  umask (0);

  /* FIXME: consider not calling getenv for SIMPLE_BACKUP_SUFFIX unless
     we'll actually use backup_suffix_string.  */
  backup_suffix_string = getenv ("SIMPLE_BACKUP_SUFFIX");

  while ((optc = getopt_long (argc, argv, "bcCsDdg:m:o:pt:TvS:Z:", long_options,
                              NULL)) != -1)
    {
      switch (optc)
        {
        case 'b':
          make_backups = true;
          if (optarg)
            version_control_string = optarg;
          break;
        case 'c':
          break;
        case 'C':
          copy_only_if_needed = true;
          break;
        case 's':
          strip_files = true;
          /* System V fork+wait does not work if SIGCHLD is ignored.  */
          signal (SIGCHLD, SIG_DFL);
          break;
        case STRIP_PROGRAM_OPTION:
          strip_program = xstrdup (optarg);
          strip_program_specified = true;
          break;
        case 'd':
          dir_arg = true;
          break;
        case 'D':
          mkdir_and_install = true;
          break;
        case 'v':
          x.verbose = true;
          break;
        case 'g':
          group_name = optarg;
          break;
        case 'm':
          specified_mode = optarg;
          break;
        case 'o':
          owner_name = optarg;
          break;
        case 'p':
          x.preserve_timestamps = true;
          break;
        case 'S':
          make_backups = true;
          backup_suffix_string = optarg;
          break;
        case 't':
          if (target_directory)
            error (EXIT_FAILURE, 0,
                   _("multiple target directories specified"));
          else
            {
              struct stat st;
              if (stat (optarg, &st) != 0)
                error (EXIT_FAILURE, errno, _("accessing %s"), quote (optarg));
              if (! S_ISDIR (st.st_mode))
                error (EXIT_FAILURE, 0, _("target %s is not a directory"),
                       quote (optarg));
            }
          target_directory = optarg;
          break;
        case 'T':
          no_target_directory = true;
          break;

        case PRESERVE_CONTEXT_OPTION_DEPRECATED:
          error (0, 0, _("WARNING: --preserve_context is deprecated; "
                         "use --preserve-context instead"));
          /* fall through */
        case PRESERVE_CONTEXT_OPTION:
          if ( ! selinux_enabled)
            {
              error (0, 0, _("WARNING: ignoring --preserve-context; "
                             "this kernel is not SELinux-enabled"));
              break;
            }
          x.preserve_security_context = true;
          use_default_selinux_context = false;
          break;
        case 'Z':
          if ( ! selinux_enabled)
            {
              error (0, 0, _("WARNING: ignoring --context (-Z); "
                             "this kernel is not SELinux-enabled"));
              break;
            }
          scontext = optarg;
          use_default_selinux_context = false;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  /* Check for invalid combinations of arguments. */
  if (dir_arg && strip_files)
    error (EXIT_FAILURE, 0,
           _("the strip option may not be used when installing a directory"));
  if (dir_arg && target_directory)
    error (EXIT_FAILURE, 0,
           _("target directory not allowed when installing a directory"));

  if (x.preserve_security_context && scontext != NULL)
    error (EXIT_FAILURE, 0,
           _("cannot force target context to %s and preserve it"),
           quote (scontext));

  if (backup_suffix_string)
    simple_backup_suffix = xstrdup (backup_suffix_string);

  x.backup_type = (make_backups
                   ? xget_version (_("backup type"),
                                   version_control_string)
                   : no_backups);

  if (scontext && setfscreatecon (scontext) < 0)
    error (EXIT_FAILURE, errno,
           _("failed to set default file creation context to %s"),
           quote (scontext));

  n_files = argc - optind;
  file = argv + optind;

  if (n_files <= ! (dir_arg || target_directory))
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
    }
  else if (! (dir_arg || target_directory))
    {
      if (2 <= n_files && target_directory_operand (file[n_files - 1]))
        target_directory = file[--n_files];
      else if (2 < n_files)
        error (EXIT_FAILURE, 0, _("target %s is not a directory"),
               quote (file[n_files - 1]));
    }

  if (specified_mode)
    {
      struct mode_change *change = mode_compile (specified_mode);
      if (!change)
        error (EXIT_FAILURE, 0, _("invalid mode %s"), quote (specified_mode));
      mode = mode_adjust (0, false, 0, change, NULL);
      dir_mode = mode_adjust (0, true, 0, change, &dir_mode_bits);
      free (change);
    }

  if (strip_program_specified && !strip_files)
    error (0, 0, _("WARNING: ignoring --strip-program option as -s option was "
                   "not specified"));

  if (copy_only_if_needed && x.preserve_timestamps)
    {
      error (0, 0, _("options --compare (-C) and --preserve-timestamps are "
                     "mutually exclusive"));
      usage (EXIT_FAILURE);
    }

  if (copy_only_if_needed && strip_files)
    {
      error (0, 0, _("options --compare (-C) and --strip are mutually "
                     "exclusive"));
      usage (EXIT_FAILURE);
    }

  if (copy_only_if_needed && extra_mode (mode))
    error (0, 0, _("the --compare (-C) option is ignored when you"
                   " specify a mode with non-permission bits"));

  get_ids ();

  if (dir_arg)
    exit_status = savewd_process_files (n_files, file, process_dir, &x);
  else
    {
      /* FIXME: it's a little gross that this initialization is
         required by copy.c::copy. */
      hash_init ();

      if (!target_directory)
        {
          if (! (mkdir_and_install
                 ? install_file_in_file_parents (file[0], file[1], &x)
                 : install_file_in_file (file[0], file[1], &x)))
            exit_status = EXIT_FAILURE;
        }
      else
        {
          int i;
          dest_info_init (&x);
          for (i = 0; i < n_files; i++)
            if (! install_file_in_dir (file[i], target_directory, &x))
              exit_status = EXIT_FAILURE;
        }
    }

  exit (exit_status);
}

