int
main (int argc, char **argv)
{
  int optchar;
  bool ok = true;
  char const *max_width_option = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  crown = tagged = split = uniform = false;
  max_width = WIDTH;
  prefix = "";
  prefix_length = prefix_lead_space = prefix_full_length = 0;

  if (argc > 1 && argv[1][0] == '-' && ISDIGIT (argv[1][1]))
    {
      /* Old option syntax; a dash followed by one or more digits.  */
      max_width_option = argv[1] + 1;

      /* Make the option we just parsed invisible to getopt.  */
      argv[1] = argv[0];
      argv++;
      argc--;
    }

  while ((optchar = getopt_long (argc, argv, "0123456789cstuw:p:",
                                 long_options, NULL))
         != -1)
    switch (optchar)
      {
      default:
        if (ISDIGIT (optchar))
          error (0, 0, _("invalid option -- %c; -WIDTH is recognized\
 only when it is the first\noption; use -w N instead"),
                 optchar);
        usage (EXIT_FAILURE);

      case 'c':
        crown = true;
        break;

      case 's':
        split = true;
        break;

      case 't':
        tagged = true;
        break;

      case 'u':
        uniform = true;
        break;

      case 'w':
        max_width_option = optarg;
        break;

      case 'p':
        set_prefix (optarg);
        break;

      case_GETOPT_HELP_CHAR;

      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

      }

  if (max_width_option)
    {
      /* Limit max_width to MAXCHARS / 2; otherwise, the resulting
         output can be quite ugly.  */
      unsigned long int tmp;
      if (! (xstrtoul (max_width_option, NULL, 10, &tmp, "") == LONGINT_OK
             && tmp <= MAXCHARS / 2))
        error (EXIT_FAILURE, 0, _("invalid width: %s"),
               quote (max_width_option));
      max_width = tmp;
    }

  best_width = max_width * (2 * (100 - LEEWAY) + 1) / 200;

  if (optind == argc)
    fmt (stdin);
  else
    {
      for (; optind < argc; optind++)
        {
          char *file = argv[optind];
          if (STREQ (file, "-"))
            fmt (stdin);
          else
            {
              FILE *in_stream;
              in_stream = fopen (file, "r");
              if (in_stream != NULL)
                {
                  fmt (in_stream);
                  if (fclose (in_stream) == EOF)
                    {
                      error (0, errno, "%s", file);
                      ok = false;
                    }
                }
              else
                {
                  error (0, errno, _("cannot open %s for reading"),
                         quote (file));
                  ok = false;
                }
            }
        }
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}

