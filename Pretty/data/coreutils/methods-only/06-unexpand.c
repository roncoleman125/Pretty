int
main (int argc, char **argv)
{
  bool have_tabval = false;
  uintmax_t tabval IF_LINT ( = 0);
  int c;

  /* If true, cancel the effect of any -a (explicit or implicit in -t),
     so that only leading blanks will be considered.  */
  bool convert_first_only = false;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;
  exit_status = EXIT_SUCCESS;
  convert_entire_line = false;
  tab_list = NULL;
  first_free_tab = 0;

  while ((c = getopt_long (argc, argv, ",0123456789at:", longopts, NULL))
         != -1)
    {
      switch (c)
        {
        case '?':
          usage (EXIT_FAILURE);
        case 'a':
          convert_entire_line = true;
          break;
        case 't':
          convert_entire_line = true;
          parse_tab_stops (optarg);
          break;
        case CONVERT_FIRST_ONLY_OPTION:
          convert_first_only = true;
          break;
        case ',':
          if (have_tabval)
            add_tab_stop (tabval);
          have_tabval = false;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          if (!have_tabval)
            {
              tabval = 0;
              have_tabval = true;
            }
          if (!DECIMAL_DIGIT_ACCUMULATE (tabval, c - '0', uintmax_t))
            error (EXIT_FAILURE, 0, _("tab stop value is too large"));
          break;
        }
    }

  if (convert_first_only)
    convert_entire_line = false;

  if (have_tabval)
    add_tab_stop (tabval);

  validate_tab_stops (tab_list, first_free_tab);

  if (first_free_tab == 0)
    tab_size = max_column_width = 8;
  else if (first_free_tab == 1)
    tab_size = tab_list[0];
  else
    tab_size = 0;

  file_list = (optind < argc ? &argv[optind] : stdin_argv);

  unexpand ();

  if (have_read_stdin && fclose (stdin) != 0)
    error (EXIT_FAILURE, errno, "-");

  exit (exit_status);
}
