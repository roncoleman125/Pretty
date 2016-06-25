int
main (int argc, char **argv)
{
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  hard_LC_COLLATE = hard_locale (LC_COLLATE);

  atexit (close_stdout);

  only_file_1 = true;
  only_file_2 = true;
  both = true;

  seen_unpairable = false;
  issued_disorder_warning[0] = issued_disorder_warning[1] = false;
  check_input_order = CHECK_ORDER_DEFAULT;

  while ((c = getopt_long (argc, argv, "123", long_options, NULL)) != -1)
    switch (c)
      {
      case '1':
        only_file_1 = false;
        break;

      case '2':
        only_file_2 = false;
        break;

      case '3':
        both = false;
        break;

      case NOCHECK_ORDER_OPTION:
        check_input_order = CHECK_ORDER_DISABLED;
        break;

      case CHECK_ORDER_OPTION:
        check_input_order = CHECK_ORDER_ENABLED;
        break;

      case OUTPUT_DELIMITER_OPTION:
        if (delimiter && !STREQ (delimiter, optarg))
          error (EXIT_FAILURE, 0, _("multiple delimiters specified"));
        delimiter = optarg;
        if (!*delimiter)
          {
            error (EXIT_FAILURE, 0, _("empty %s not allowed"),
                   quote ("--output-delimiter"));
          }
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

  if (2 < argc - optind)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 2]));
      usage (EXIT_FAILURE);
    }

  /* The default delimiter is a TAB. */
  if (!delimiter)
    delimiter = "\t";

  compare_files (argv + optind);

  if (issued_disorder_warning[0] || issued_disorder_warning[1])
    exit (EXIT_FAILURE);
  else
    exit (EXIT_SUCCESS);
}
