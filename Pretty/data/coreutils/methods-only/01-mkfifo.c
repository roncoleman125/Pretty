int
main (int argc, char **argv)
{
  mode_t newmode;
  char const *specified_mode = NULL;
  int exit_status = EXIT_SUCCESS;
  int optc;
  security_context_t scontext = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "m:Z:", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'm':
          specified_mode = optarg;
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

  newmode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (specified_mode)
    {
      struct mode_change *change = mode_compile (specified_mode);
      if (!change)
        error (EXIT_FAILURE, 0, _("invalid mode"));
      newmode = mode_adjust (newmode, false, umask (0), change, NULL);
      free (change);
      if (newmode & ~S_IRWXUGO)
        error (EXIT_FAILURE, 0,
               _("mode must specify only file permission bits"));
    }

  for (; optind < argc; ++optind)
    if (mkfifo (argv[optind], newmode) != 0)
      {
        error (0, errno, _("cannot create fifo %s"), quote (argv[optind]));
        exit_status = EXIT_FAILURE;
      }

  exit (exit_status);
}
