int
main (int argc, char **argv)
{
  int optc;
  int n_users;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "sfwiqbhlp", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 's':
          do_short_format = true;
          break;

        case 'l':
          do_short_format = false;
          break;

        case 'f':
          include_heading = false;
          break;

        case 'w':
          include_fullname = false;
          break;

        case 'i':
          include_fullname = false;
          include_where = false;
          break;

        case 'q':
          include_fullname = false;
          include_where = false;
          include_idle = false;
          break;

        case 'h':
          include_project = false;
          break;

        case 'p':
          include_plan = false;
          break;

        case 'b':
          include_home_and_shell = false;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  n_users = argc - optind;

  if (!do_short_format && n_users == 0)
    {
      error (0, 0, _("no username specified; at least one must be\
 specified when using -l"));
      usage (EXIT_FAILURE);
    }

  if (do_short_format)
    short_pinky (UTMP_FILE, n_users, argv + optind);
  else
    long_pinky (n_users, argv + optind);

  exit (EXIT_SUCCESS);
}
