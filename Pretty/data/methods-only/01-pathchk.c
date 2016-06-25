int
main (int argc, char **argv)
{
  bool ok = true;
  bool check_basic_portability = false;
  bool check_extra_portability = false;
  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "+pP", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case PORTABILITY_OPTION:
          check_basic_portability = true;
          check_extra_portability = true;
          break;

        case 'p':
          check_basic_portability = true;
          break;

        case 'P':
          check_extra_portability = true;
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

  for (; optind < argc; ++optind)
    ok &= validate_file_name (argv[optind],
                              check_basic_portability, check_extra_portability);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}

