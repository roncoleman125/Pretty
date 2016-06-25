int
main (int argc, char **argv)
{
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;
  exit_status = EXIT_SUCCESS;
  convert_entire_line = true;
  tab_list = NULL;
  first_free_tab = 0;

  while ((c = getopt_long (argc, argv, shortopts, longopts, NULL)) != -1)
    {
      switch (c)
        {
        case 'i':
          convert_entire_line = false;
          break;

        case 't':
          parse_tab_stops (optarg);
          break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          if (optarg)
            parse_tab_stops (optarg - 1);
          else
            {
              char tab_stop[2];
              tab_stop[0] = c;
              tab_stop[1] = '\0';
              parse_tab_stops (tab_stop);
            }
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  validate_tab_stops (tab_list, first_free_tab);

  if (first_free_tab == 0)
    tab_size = 8;
  else if (first_free_tab == 1)
    tab_size = tab_list[0];
  else
    tab_size = 0;

  file_list = (optind < argc ? &argv[optind] : stdin_argv);

  expand ();

  if (have_read_stdin && fclose (stdin) != 0)
    error (EXIT_FAILURE, errno, "-");

  exit (exit_status);
}
