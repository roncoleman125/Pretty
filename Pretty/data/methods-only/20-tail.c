static void
parse_options (int argc, char **argv,
               uintmax_t *n_units, enum header_mode *header_mode,
               double *sleep_interval)
{
  int c;

  while ((c = getopt_long (argc, argv, "c:n:fFqs:v0123456789",
                           long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case 'F':
          forever = true;
          follow_mode = Follow_name;
          reopen_inaccessible_files = true;
          break;

        case 'c':
        case 'n':
          count_lines = (c == 'n');
          if (*optarg == '+')
            from_start = true;
          else if (*optarg == '-')
            ++optarg;

          {
            strtol_error s_err;
            s_err = xstrtoumax (optarg, NULL, 10, n_units, "bkKmMGTPEZY0");
            if (s_err != LONGINT_OK)
              {
                error (EXIT_FAILURE, 0, "%s: %s", optarg,
                       (c == 'n'
                        ? _("invalid number of lines")
                        : _("invalid number of bytes")));
              }
          }
          break;

        case 'f':
        case LONG_FOLLOW_OPTION:
          forever = true;
          if (optarg == NULL)
            follow_mode = DEFAULT_FOLLOW_MODE;
          else
            follow_mode = XARGMATCH ("--follow", optarg,
                                     follow_mode_string, follow_mode_map);
          break;

        case RETRY_OPTION:
          reopen_inaccessible_files = true;
          break;

        case MAX_UNCHANGED_STATS_OPTION:
          /* --max-unchanged-stats=N */
          if (xstrtoumax (optarg, NULL, 10,
                          &max_n_unchanged_stats_between_opens,
                          "")
              != LONGINT_OK)
            {
              error (EXIT_FAILURE, 0,
               _("%s: invalid maximum number of unchanged stats between opens"),
                     optarg);
            }
          break;

        case DISABLE_INOTIFY_OPTION:
          disable_inotify = true;
          break;

        case PID_OPTION:
          {
            strtol_error s_err;
            unsigned long int tmp_ulong;
            s_err = xstrtoul (optarg, NULL, 10, &tmp_ulong, "");
            if (s_err != LONGINT_OK || tmp_ulong > PID_T_MAX)
              {
                error (EXIT_FAILURE, 0, _("%s: invalid PID"), optarg);
              }
            pid = tmp_ulong;
          }
          break;

        case PRESUME_INPUT_PIPE_OPTION:
          presume_input_pipe = true;
          break;

        case 'q':
          *header_mode = never;
          break;

        case 's':
          {
            double s;
            if (! (xstrtod (optarg, NULL, &s, c_strtod) && 0 <= s))
              error (EXIT_FAILURE, 0,
                     _("%s: invalid number of seconds"), optarg);
            *sleep_interval = s;
          }
          break;

        case 'v':
          *header_mode = always;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          error (EXIT_FAILURE, 0,
                 _("option used in invalid context -- %c"), c);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (reopen_inaccessible_files && follow_mode != Follow_name)
    error (0, 0, _("warning: --retry is useful mainly when following by name"));

  if (pid && !forever)
    error (0, 0,
           _("warning: PID ignored; --pid=PID is useful only when following"));
  else if (pid && kill (pid, 0) != 0 && errno == ENOSYS)
    {
      error (0, 0, _("warning: --pid=PID is not supported on this system"));
      pid = 0;
    }
}

