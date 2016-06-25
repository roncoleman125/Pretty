int
main (int argc, char *argv[])
{
  int c;
  int i;
  bool fs = false;
  bool terse = false;
  char *format = NULL;
  char *format2;
  bool ok = true;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  struct lconv const *locale = localeconv ();
  decimal_point = (locale->decimal_point[0] ? locale->decimal_point : ".");
  decimal_point_len = strlen (decimal_point);

  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "c:fLt", long_options, NULL)) != -1)
    {
      switch (c)
        {
        case PRINTF_OPTION:
          format = optarg;
          interpret_backslash_escapes = true;
          trailing_delim = "";
          break;

        case 'c':
          format = optarg;
          interpret_backslash_escapes = false;
          trailing_delim = "\n";
          break;

        case 'L':
          follow_links = true;
          break;

        case 'f':
          fs = true;
          break;

        case 't':
          terse = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (argc == optind)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  if (format)
    format2 = format;
  else
    {
      format = default_format (fs, terse, false);
      format2 = default_format (fs, terse, true);
    }

  for (i = optind; i < argc; i++)
    ok &= (fs
           ? do_statfs (argv[i], terse, format)
           : do_stat (argv[i], terse, format, format2));

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
