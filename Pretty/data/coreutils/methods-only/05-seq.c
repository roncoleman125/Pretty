int
main (int argc, char **argv)
{
  int optc;
  operand first = { 1, 1, 0 };
  operand step = { 1, 1, 0 };
  operand last;
  struct layout layout = { 0, 0 };

  /* The printf(3) format used for output.  */
  char const *format_str = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  equal_width = false;
  separator = "\n";

  /* We have to handle negative numbers in the command line but this
     conflicts with the command line arguments.  So explicitly check first
     whether the next argument looks like a negative number.  */
  while (optind < argc)
    {
      if (argv[optind][0] == '-'
          && ((optc = argv[optind][1]) == '.' || ISDIGIT (optc)))
        {
          /* means negative number */
          break;
        }

      optc = getopt_long (argc, argv, "+f:s:w", long_options, NULL);
      if (optc == -1)
        break;

      switch (optc)
        {
        case 'f':
          format_str = optarg;
          break;

        case 's':
          separator = optarg;
          break;

        case 'w':
          equal_width = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (argc - optind < 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  if (3 < argc - optind)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 3]));
      usage (EXIT_FAILURE);
    }

  if (format_str)
    format_str = long_double_format (format_str, &layout);

  last = scan_arg (argv[optind++]);

  if (optind < argc)
    {
      first = last;
      last = scan_arg (argv[optind++]);

      if (optind < argc)
        {
          step = last;
          last = scan_arg (argv[optind++]);
        }
    }

  if (format_str != NULL && equal_width)
    {
      error (0, 0, _("\
format string may not be specified when printing equal width strings"));
      usage (EXIT_FAILURE);
    }

  if (format_str == NULL)
    format_str = get_default_format (first, step, last);

  print_numbers (format_str, layout, first.value, step.value, last.value);

  exit (EXIT_SUCCESS);
}
