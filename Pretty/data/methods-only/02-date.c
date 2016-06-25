int
main (int argc, char **argv)
{
  int optc;
  const char *datestr = NULL;
  const char *set_datestr = NULL;
  struct timespec when;
  bool set_date = false;
  char const *format = NULL;
  char *batch_file = NULL;
  char *reference = NULL;
  struct stat refstats;
  bool ok;
  int option_specified_date;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, short_options, long_options, NULL))
         != -1)
    {
      char const *new_format = NULL;

      switch (optc)
        {
        case 'd':
          datestr = optarg;
          break;
        case 'f':
          batch_file = optarg;
          break;
        case RFC_3339_OPTION:
          {
            static char const rfc_3339_format[][32] =
              {
                "%Y-%m-%d",
                "%Y-%m-%d %H:%M:%S%:z",
                "%Y-%m-%d %H:%M:%S.%N%:z"
              };
            enum Time_spec i =
              XARGMATCH ("--rfc-3339", optarg,
                         time_spec_string + 2, time_spec + 2);
            new_format = rfc_3339_format[i];
            break;
          }
        case 'I':
          {
            static char const iso_8601_format[][32] =
              {
                "%Y-%m-%d",
                "%Y-%m-%dT%H:%M:%S%z",
                "%Y-%m-%dT%H:%M:%S,%N%z",
                "%Y-%m-%dT%H%z",
                "%Y-%m-%dT%H:%M%z"
              };
            enum Time_spec i =
              (optarg
               ? XARGMATCH ("--iso-8601", optarg, time_spec_string, time_spec)
               : TIME_SPEC_DATE);
            new_format = iso_8601_format[i];
            break;
          }
        case 'r':
          reference = optarg;
          break;
        case 'R':
          new_format = rfc_2822_format;
          break;
        case 's':
          set_datestr = optarg;
          set_date = true;
          break;
        case 'u':
          /* POSIX says that `date -u' is equivalent to setting the TZ
             environment variable, so this option should do nothing other
             than setting TZ.  */
          if (putenv (bad_cast ("TZ=UTC0")) != 0)
            xalloc_die ();
          TZSET;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }

      if (new_format)
        {
          if (format)
            error (EXIT_FAILURE, 0, _("multiple output formats specified"));
          format = new_format;
        }
    }

  option_specified_date = ((datestr ? 1 : 0)
                           + (batch_file ? 1 : 0)
                           + (reference ? 1 : 0));

  if (option_specified_date > 1)
    {
      error (0, 0,
        _("the options to specify dates for printing are mutually exclusive"));
      usage (EXIT_FAILURE);
    }

  if (set_date && option_specified_date)
    {
      error (0, 0,
          _("the options to print and set the time may not be used together"));
      usage (EXIT_FAILURE);
    }

  if (optind < argc)
    {
      if (optind + 1 < argc)
        {
          error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
          usage (EXIT_FAILURE);
        }

      if (argv[optind][0] == '+')
        {
          if (format)
            error (EXIT_FAILURE, 0, _("multiple output formats specified"));
          format = argv[optind++] + 1;
        }
      else if (set_date || option_specified_date)
        {
          error (0, 0,
                 _("the argument %s lacks a leading `+';\n"
                   "when using an option to specify date(s), any non-option\n"
                   "argument must be a format string beginning with `+'"),
                 quote (argv[optind]));
          usage (EXIT_FAILURE);
        }
    }

  if (!format)
    {
      format = DATE_FMT_LANGINFO ();
      if (! *format)
        {
          /* Do not wrap the following literal format string with _(...).
             For example, suppose LC_ALL is unset, LC_TIME=POSIX,
             and LANG="ko_KR".  In that case, POSIX says that LC_TIME
             determines the format and contents of date and time strings
             written by date, which means "date" must generate output
             using the POSIX locale; but adding _() would cause "date"
             to use a Korean translation of the format.  */
          format = "%a %b %e %H:%M:%S %Z %Y";
        }
    }

  if (batch_file != NULL)
    ok = batch_convert (batch_file, format);
  else
    {
      bool valid_date = true;
      ok = true;

      if (!option_specified_date && !set_date)
        {
          if (optind < argc)
            {
              /* Prepare to set system clock to the specified date/time
                 given in the POSIX-format.  */
              set_date = true;
              datestr = argv[optind];
              valid_date = posixtime (&when.tv_sec,
                                      datestr,
                                      (PDS_TRAILING_YEAR
                                       | PDS_CENTURY | PDS_SECONDS));
              when.tv_nsec = 0; /* FIXME: posixtime should set this.  */
            }
          else
            {
              /* Prepare to print the current date/time.  */
              gettime (&when);
            }
        }
      else
        {
          /* (option_specified_date || set_date) */
          if (reference != NULL)
            {
              if (stat (reference, &refstats) != 0)
                error (EXIT_FAILURE, errno, "%s", reference);
              when = get_stat_mtime (&refstats);
            }
          else
            {
              if (set_datestr)
                datestr = set_datestr;
              valid_date = parse_datetime (&when, datestr, NULL);
            }
        }

      if (! valid_date)
        error (EXIT_FAILURE, 0, _("invalid date %s"), quote (datestr));

      if (set_date)
        {
          /* Set the system clock to the specified date, then regardless of
             the success of that operation, format and print that date.  */
          if (settime (&when) != 0)
            {
              error (0, errno, _("cannot set date"));
              ok = false;
            }
        }

      ok &= show_date (format, when);
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}

