int
main (int argc, char **argv)
{
  int optc = 0;
  bool posixly_correct = (getenv ("POSIXLY_CORRECT") != NULL);
  enum Skip_field_option_type skip_field_option_type = SFO_NONE;
  int nfiles = 0;
  char const *file[2];
  char delimiter = '\n';	/* change with --zero-terminated, -z */

  file[0] = file[1] = "-";
  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  hard_LC_COLLATE = hard_locale (LC_COLLATE);

  atexit (close_stdout);

  skip_chars = 0;
  skip_fields = 0;
  check_chars = SIZE_MAX;
  output_unique = output_first_repeated = true;
  output_later_repeated = false;
  countmode = count_none;
  delimit_groups = DM_NONE;

  while (true)
    {
      /* Parse an operand with leading "+" as a file after "--" was
         seen; or if pedantic and a file was seen; or if not
         obsolete.  */

      if (optc == -1
          || (posixly_correct && nfiles != 0)
          || ((optc = getopt_long (argc, argv,
                                   "-0123456789Dcdf:is:uw:z", longopts, NULL))
              == -1))
        {
          if (argc <= optind)
            break;
          if (nfiles == 2)
            {
              error (0, 0, _("extra operand %s"), quote (argv[optind]));
              usage (EXIT_FAILURE);
            }
          file[nfiles++] = argv[optind++];
        }
      else switch (optc)
        {
        case 1:
          {
            unsigned long int size;
            if (optarg[0] == '+'
                && posix2_version () < 200112
                && xstrtoul (optarg, NULL, 10, &size, "") == LONGINT_OK
                && size <= SIZE_MAX)
              skip_chars = size;
            else if (nfiles == 2)
              {
                error (0, 0, _("extra operand %s"), quote (optarg));
                usage (EXIT_FAILURE);
              }
            else
              file[nfiles++] = optarg;
          }
          break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          {
            if (skip_field_option_type == SFO_NEW)
              skip_fields = 0;

            if (!DECIMAL_DIGIT_ACCUMULATE (skip_fields, optc - '0', size_t))
              skip_fields = SIZE_MAX;

            skip_field_option_type = SFO_OBSOLETE;
          }
          break;

        case 'c':
          countmode = count_occurrences;
          break;

        case 'd':
          output_unique = false;
          break;

        case 'D':
          output_unique = false;
          output_later_repeated = true;
          if (optarg == NULL)
            delimit_groups = DM_NONE;
          else
            delimit_groups = XARGMATCH ("--all-repeated", optarg,
                                        delimit_method_string,
                                        delimit_method_map);
          break;

        case 'f':
          skip_field_option_type = SFO_NEW;
          skip_fields = size_opt (optarg,
                                  N_("invalid number of fields to skip"));
          break;

        case 'i':
          ignore_case = true;
          break;

        case 's':
          skip_chars = size_opt (optarg,
                                 N_("invalid number of bytes to skip"));
          break;

        case 'u':
          output_first_repeated = false;
          break;

        case 'w':
          check_chars = size_opt (optarg,
                                  N_("invalid number of bytes to compare"));
          break;

        case 'z':
          delimiter = '\0';
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (countmode == count_occurrences && output_later_repeated)
    {
      error (0, 0,
           _("printing all duplicated lines and repeat counts is meaningless"));
      usage (EXIT_FAILURE);
    }

  check_file (file[0], file[1], delimiter);

  exit (EXIT_SUCCESS);
}
