int
main (int argc, char **argv)
{
  const char *specified_mode = NULL;
  int optc;
  security_context_t scontext = NULL;
  struct mkdir_options options;

  options.make_ancestor_function = NULL;
  options.mode = S_IRWXUGO;
  options.mode_bits = 0;
  options.created_directory_format = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "pm:vZ:", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'p':
          options.make_ancestor_function = make_ancestor;
          break;
        case 'm':
          specified_mode = optarg;
          break;
        case 'v': /* --verbose  */
          options.created_directory_format = _("created directory %s");
          break;
        case 'Z':
          scontext = optarg;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (optind == argc)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  if (scontext && setfscreatecon (scontext) < 0)
    error (EXIT_FAILURE, errno,
           _("failed to set default file creation context to %s"),
           quote (scontext));

  if (options.make_ancestor_function || specified_mode)
    {
      mode_t umask_value = umask (0);

      options.ancestor_mode = (S_IRWXUGO & ~umask_value) | (S_IWUSR | S_IXUSR);

      if (specified_mode)
        {
          struct mode_change *change = mode_compile (specified_mode);
          if (!change)
            error (EXIT_FAILURE, 0, _("invalid mode %s"),
                   quote (specified_mode));
          options.mode = mode_adjust (S_IRWXUGO, true, umask_value, change,
                                      &options.mode_bits);
          free (change);
        }
      else
        options.mode = S_IRWXUGO & ~umask_value;
    }

  exit (savewd_process_files (argc - optind, argv + optind,
                              process_dir, &options));
}
