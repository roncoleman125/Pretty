int
main (int argc, char **argv)
{
  int optc;
  bool list = false;
  bool table = false;
  int signum = -1;
  char signame[SIG2STR_MAX];

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, short_options, long_options, NULL))
         != -1)
    switch (optc)
      {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        if (optind != 2)
          {
            /* This option is actually a process-id.  */
            optind--;
            goto no_more_options;
          }
        /* Fall through.  */
      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y':
      case 'Z':
        if (! optarg)
          optarg = argv[optind - 1] + strlen (argv[optind - 1]);
        if (optarg != argv[optind - 1] + 2)
          {
            error (0, 0, _("invalid option -- %c"), optc);
            usage (EXIT_FAILURE);
          }
        optarg--;
        /* Fall through.  */
      case 'n': /* -n is not documented, but is for Bash compatibility.  */
      case 's':
        if (0 <= signum)
          {
            error (0, 0, _("%s: multiple signals specified"), optarg);
            usage (EXIT_FAILURE);
          }
        signum = operand2sig (optarg, signame);
        if (signum < 0)
          usage (EXIT_FAILURE);
        break;

      case 't':
        table = true;
        /* Fall through.  */
      case 'l':
        if (list)
          {
            error (0, 0, _("multiple -l or -t options specified"));
            usage (EXIT_FAILURE);
          }
        list = true;
        break;

      case_GETOPT_HELP_CHAR;
      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
      default:
        usage (EXIT_FAILURE);
      }
 no_more_options:;

  if (signum < 0)
    signum = SIGTERM;
  else if (list)
    {
      error (0, 0, _("cannot combine signal with -l or -t"));
      usage (EXIT_FAILURE);
    }

  if ( ! list && argc <= optind)
    {
      error (0, 0, _("no process ID specified"));
      usage (EXIT_FAILURE);
    }

  return (list
          ? list_signals (table, optind < argc ? argv + optind : NULL)
          : send_signals (signum, argv + optind));
}
