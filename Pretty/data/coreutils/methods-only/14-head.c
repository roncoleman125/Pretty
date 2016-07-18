int
main (int argc, char **argv)
{
  enum header_mode header_mode = multiple_files;
  bool ok = true;
  int c;
  size_t i;

  /* Number of items to print. */
  uintmax_t n_units = DEFAULT_NUMBER;

  /* If true, interpret the numeric argument as the number of lines.
     Otherwise, interpret it as the number of bytes.  */
  bool count_lines = true;

  /* Elide the specified number of lines or bytes, counting from
     the end of the file.  */
  bool elide_from_end = false;

  /* Initializer for file_list if no file-arguments
     were specified on the command line.  */
  static char const *const default_file_list[] = {"-", NULL};
  char const *const *file_list;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;

  print_headers = false;

  if (1 < argc && argv[1][0] == '-' && ISDIGIT (argv[1][1]))
    {
      char *a = argv[1];
      char *n_string = ++a;
      char *end_n_string;
      char multiplier_char = 0;

      /* Old option syntax; a dash, one or more digits, and one or
         more option letters.  Move past the number. */
      do ++a;
      while (ISDIGIT (*a));

      /* Pointer to the byte after the last digit.  */
      end_n_string = a;

      /* Parse any appended option letters. */
      for (; *a; a++)
        {
          switch (*a)
            {
            case 'c':
              count_lines = false;
              multiplier_char = 0;
              break;

            case 'b':
            case 'k':
            case 'm':
              count_lines = false;
              multiplier_char = *a;
              break;

            case 'l':
              count_lines = true;
              break;

            case 'q':
              header_mode = never;
              break;

            case 'v':
              header_mode = always;
              break;

            default:
              error (0, 0, _("invalid trailing option -- %c"), *a);
              usage (EXIT_FAILURE);
            }
        }

      /* Append the multiplier character (if any) onto the end of
         the digit string.  Then add NUL byte if necessary.  */
      *end_n_string = multiplier_char;
      if (multiplier_char)
        *(++end_n_string) = 0;

      n_units = string_to_integer (count_lines, n_string);

      /* Make the options we just parsed invisible to getopt. */
      argv[1] = argv[0];
      argv++;
      argc--;
    }

  while ((c = getopt_long (argc, argv, "c:n:qv0123456789", long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case PRESUME_INPUT_PIPE_OPTION:
          presume_input_pipe = true;
          break;

        case 'c':
          count_lines = false;
          elide_from_end = (*optarg == '-');
          if (elide_from_end)
            ++optarg;
          n_units = string_to_integer (count_lines, optarg);
          break;

        case 'n':
          count_lines = true;
          elide_from_end = (*optarg == '-');
          if (elide_from_end)
            ++optarg;
          n_units = string_to_integer (count_lines, optarg);
          break;

        case 'q':
          header_mode = never;
          break;

        case 'v':
          header_mode = always;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          if (ISDIGIT (c))
            error (0, 0, _("invalid trailing option -- %c"), c);
          usage (EXIT_FAILURE);
        }
    }

  if (header_mode == always
      || (header_mode == multiple_files && optind < argc - 1))
    print_headers = true;

  if ( ! count_lines && elide_from_end && OFF_T_MAX < n_units)
    {
      char umax_buf[INT_BUFSIZE_BOUND (n_units)];
      error (EXIT_FAILURE, 0, _("%s: number of bytes is too large"),
             umaxtostr (n_units, umax_buf));
    }

  file_list = (optind < argc
               ? (char const *const *) &argv[optind]
               : default_file_list);

  if (O_BINARY && ! isatty (STDOUT_FILENO))
    xfreopen (NULL, "wb", stdout);

  for (i = 0; file_list[i]; ++i)
    ok &= head_file (file_list[i], n_units, count_lines, elide_from_end);

  if (have_read_stdin && close (STDIN_FILENO) < 0)
    error (EXIT_FAILURE, errno, "-");

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
