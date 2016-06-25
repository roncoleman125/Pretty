int
main (int argc, char **argv)
{
  int optc_status;
  int prev_optc_status = MUST_BE_OPERAND;
  int operand_status[2];
  int joption_count[2] = { 0, 0 };
  char *names[2];
  FILE *fp1, *fp2;
  int optc;
  int nfiles = 0;
  int i;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  hard_LC_COLLATE = hard_locale (LC_COLLATE);

  atexit (close_stdout);
  atexit (free_spareline);

  print_pairables = true;
  seen_unpairable = false;
  issued_disorder_warning[0] = issued_disorder_warning[1] = false;
  check_input_order = CHECK_ORDER_DEFAULT;

  while ((optc = getopt_long (argc, argv, "-a:e:i1:2:j:o:t:v:",
                              longopts, NULL))
         != -1)
    {
      optc_status = MUST_BE_OPERAND;

      switch (optc)
        {
        case 'v':
            print_pairables = false;
            /* Fall through.  */

        case 'a':
          {
            unsigned long int val;
            if (xstrtoul (optarg, NULL, 10, &val, "") != LONGINT_OK
                || (val != 1 && val != 2))
              error (EXIT_FAILURE, 0,
                     _("invalid field number: %s"), quote (optarg));
            if (val == 1)
              print_unpairables_1 = true;
            else
              print_unpairables_2 = true;
          }
          break;

        case 'e':
          if (empty_filler && ! STREQ (empty_filler, optarg))
            error (EXIT_FAILURE, 0,
                   _("conflicting empty-field replacement strings"));
          empty_filler = optarg;
          break;

        case 'i':
          ignore_case = true;
          break;

        case '1':
          set_join_field (&join_field_1, string_to_join_field (optarg));
          break;

        case '2':
          set_join_field (&join_field_2, string_to_join_field (optarg));
          break;

        case 'j':
          if ((optarg[0] == '1' || optarg[0] == '2') && !optarg[1]
              && optarg == argv[optind - 1] + 2)
            {
              /* The argument was either "-j1" or "-j2".  */
              bool is_j2 = (optarg[0] == '2');
              joption_count[is_j2]++;
              optc_status = MIGHT_BE_J1_ARG + is_j2;
            }
          else
            {
              set_join_field (&join_field_1, string_to_join_field (optarg));
              set_join_field (&join_field_2, join_field_1);
            }
          break;

        case 'o':
          if (STREQ (optarg, "auto"))
            autoformat = true;
          else
            {
              add_field_list (optarg);
              optc_status = MIGHT_BE_O_ARG;
            }
          break;

        case 't':
          {
            unsigned char newtab = optarg[0];
            if (! newtab)
              newtab = '\n'; /* '' => process the whole line.  */
            else if (optarg[1])
              {
                if (STREQ (optarg, "\\0"))
                  newtab = '\0';
                else
                  error (EXIT_FAILURE, 0, _("multi-character tab %s"),
                         quote (optarg));
              }
            if (0 <= tab && tab != newtab)
              error (EXIT_FAILURE, 0, _("incompatible tabs"));
            tab = newtab;
          }
          break;

        case NOCHECK_ORDER_OPTION:
          check_input_order = CHECK_ORDER_DISABLED;
          break;

        case CHECK_ORDER_OPTION:
          check_input_order = CHECK_ORDER_ENABLED;
          break;

        case 1:		/* Non-option argument.  */
          add_file_name (optarg, names, operand_status, joption_count,
                         &nfiles, &prev_optc_status, &optc_status);
          break;

        case HEADER_LINE_OPTION:
          join_header_lines = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }

      prev_optc_status = optc_status;
    }

  /* Process any operands after "--".  */
  prev_optc_status = MUST_BE_OPERAND;
  while (optind < argc)
    add_file_name (argv[optind++], names, operand_status, joption_count,
                   &nfiles, &prev_optc_status, &optc_status);

  if (nfiles != 2)
    {
      if (nfiles == 0)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      usage (EXIT_FAILURE);
    }

  /* If "-j1" was specified and it turns out not to have had an argument,
     treat it as "-j 1".  Likewise for -j2.  */
  for (i = 0; i < 2; i++)
    if (joption_count[i] != 0)
      {
        set_join_field (&join_field_1, i);
        set_join_field (&join_field_2, i);
      }

  if (join_field_1 == SIZE_MAX)
    join_field_1 = 0;
  if (join_field_2 == SIZE_MAX)
    join_field_2 = 0;

  fp1 = STREQ (names[0], "-") ? stdin : fopen (names[0], "r");
  if (!fp1)
    error (EXIT_FAILURE, errno, "%s", names[0]);
  fp2 = STREQ (names[1], "-") ? stdin : fopen (names[1], "r");
  if (!fp2)
    error (EXIT_FAILURE, errno, "%s", names[1]);
  if (fp1 == fp2)
    error (EXIT_FAILURE, errno, _("both files cannot be standard input"));
  join (fp1, fp2);

  if (fclose (fp1) != 0)
    error (EXIT_FAILURE, errno, "%s", names[0]);
  if (fclose (fp2) != 0)
    error (EXIT_FAILURE, errno, "%s", names[1]);

  if (issued_disorder_warning[0] || issued_disorder_warning[1])
    exit (EXIT_FAILURE);
  else
    exit (EXIT_SUCCESS);
}
