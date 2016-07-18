int
main (int argc, char **argv)
{
  int optc;
  unsigned long int val;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  atexit (close_stdout);

  global_argv = argv;
  controls = NULL;
  control_used = 0;
  suppress_count = false;
  remove_files = true;
  prefix = DEFAULT_PREFIX;

  while ((optc = getopt_long (argc, argv, "f:b:kn:sqz", longopts, NULL)) != -1)
    switch (optc)
      {
      case 'f':
  textdomain (PACKAGE);

        prefix = optarg;
        break;

      case 'b':
        suffix = optarg;
        break;

      case 'k':
        remove_files = false;
        break;

      case 'n':
        if (xstrtoul (optarg, NULL, 10, &val, "") != LONGINT_OK
            || MIN (INT_MAX, SIZE_MAX) < val)
          error (EXIT_FAILURE, 0, _("%s: invalid number"), optarg);
        digits = val;
        break;

      case 's':
      case 'q':
        suppress_count = true;
        break;

      case 'z':
        elide_empty_files = true;
        break;

      case_GETOPT_HELP_CHAR;

      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

      default:
        usage (EXIT_FAILURE);
      }

  if (argc - optind < 2)
    {
      if (argc <= optind)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      usage (EXIT_FAILURE);
    }

  size_t prefix_len = strlen (prefix);
  size_t max_digit_string_len
    = (suffix
       ? max_out (suffix)
       : MAX (INT_STRLEN_BOUND (unsigned int), digits));
  if (SIZE_MAX - 1 - prefix_len < max_digit_string_len)
    xalloc_die ();
  filename_space = xmalloc (prefix_len + max_digit_string_len + 1);

  set_input_file (argv[optind++]);

  parse_patterns (argc, optind, argv);

  {
    int i;
    static int const sig[] =
      {
        /* The usual suspects.  */
        SIGALRM, SIGHUP, SIGINT, SIGPIPE, SIGQUIT, SIGTERM,
        SIGPOLL,
        SIGPROF,
        SIGVTALRM,
        SIGXCPU,
        SIGXFSZ,
      };
    enum { nsigs = ARRAY_CARDINALITY (sig) };

    struct sigaction act;

    sigemptyset (&caught_signals);
    for (i = 0; i < nsigs; i++)
      {
        sigaction (sig[i], NULL, &act);
        if (act.sa_handler != SIG_IGN)
          sigaddset (&caught_signals, sig[i]);
      }

    act.sa_handler = interrupt_handler;
    act.sa_mask = caught_signals;
    act.sa_flags = SA_NODEFER | SA_RESETHAND;

    for (i = 0; i < nsigs; i++)
      if (sigismember (&caught_signals, sig[i]))
        sigaction (sig[i], &act, NULL);
  }

  split_file ();

  if (close (STDIN_FILENO) != 0)
    {
      error (0, errno, _("read error"));
      cleanup_fatal ();
    }

  exit (EXIT_SUCCESS);
}

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... FILE PATTERN...\n\
"),
              program_name);
      fputs (_("\
Output pieces of FILE separated by PATTERN(s) to files `xx00', `xx01', ...,\n\
and output byte counts of each piece to standard output.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -b, --suffix-format=FORMAT  use sprintf FORMAT instead of %02d\n\
  -f, --prefix=PREFIX        use PREFIX instead of `xx'\n\
  -k, --keep-files           do not remove output files on errors\n\
"), stdout);
      fputs (_("\
  -n, --digits=DIGITS        use specified number of digits instead of 2\n\
  -s, --quiet, --silent      do not print counts of output file sizes\n\
  -z, --elide-empty-files    remove empty output files\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Read standard input if FILE is -.  Each PATTERN may be:\n\
"), stdout);
      fputs (_("\
\n\
  INTEGER            copy up to but not including specified line number\n\
  /REGEXP/[OFFSET]   copy up to but not including a matching line\n\
  %REGEXP%[OFFSET]   skip to, but not including a matching line\n\
  {INTEGER}          repeat the previous pattern specified number of times\n\
  {*}                repeat the previous pattern as many times as possible\n\
\n\
A line OFFSET is a required `+' or `-' followed by a positive integer.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}
