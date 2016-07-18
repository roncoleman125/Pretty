int
main (int argc, char **argv)
{
  bool echo = false;
  size_t lo_input = SIZE_MAX;
  size_t hi_input = 0;
  size_t head_lines = SIZE_MAX;
  char const *outfile = NULL;
  char *random_source = NULL;
  char eolbyte = '\n';
  char **input_lines = NULL;

  int optc;
  int n_operands;
  char **operand;
  size_t n_lines;
  char **line;
  struct randint_source *randint_source;
  size_t *permutation;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "ei:n:o:z", long_opts, NULL)) != -1)
    switch (optc)
      {
      case 'e':
        echo = true;
        break;

      case 'i':
        {
          unsigned long int argval = 0;
          char *p = strchr (optarg, '-');
          char const *hi_optarg = optarg;
          bool invalid = !p;

          if (input_numbers_option_used (lo_input, hi_input))
            error (EXIT_FAILURE, 0, _("multiple -i options specified"));

          if (p)
            {
              *p = '\0';
              invalid = ((xstrtoul (optarg, NULL, 10, &argval, NULL)
                          != LONGINT_OK)
                         || SIZE_MAX < argval);
              *p = '-';
              lo_input = argval;
              hi_optarg = p + 1;
            }

          invalid |= ((xstrtoul (hi_optarg, NULL, 10, &argval, NULL)
                       != LONGINT_OK)
                      || SIZE_MAX < argval);
          hi_input = argval;
          n_lines = hi_input - lo_input + 1;
          invalid |= ((lo_input <= hi_input) == (n_lines == 0));
          if (invalid)
            error (EXIT_FAILURE, 0, _("invalid input range %s"),
                   quote (optarg));
        }
        break;

      case 'n':
        {
          unsigned long int argval;
          strtol_error e = xstrtoul (optarg, NULL, 10, &argval, NULL);

          if (e == LONGINT_OK)
            head_lines = MIN (head_lines, argval);
          else if (e != LONGINT_OVERFLOW)
            error (EXIT_FAILURE, 0, _("invalid line count %s"),
                   quote (optarg));
        }
        break;

      case 'o':
        if (outfile && !STREQ (outfile, optarg))
          error (EXIT_FAILURE, 0, _("multiple output files specified"));
        outfile = optarg;
        break;

      case RANDOM_SOURCE_OPTION:
        if (random_source && !STREQ (random_source, optarg))
          error (EXIT_FAILURE, 0, _("multiple random sources specified"));
        random_source = optarg;
        break;

      case 'z':
        eolbyte = '\0';
        break;

      case_GETOPT_HELP_CHAR;
      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
      default:
        usage (EXIT_FAILURE);
      }

  n_operands = argc - optind;
  operand = argv + optind;

  if (echo)
    {
      if (input_numbers_option_used (lo_input, hi_input))
        error (EXIT_FAILURE, 0, _("cannot combine -e and -i options"));
      input_from_argv (operand, n_operands, eolbyte);
      n_lines = n_operands;
      line = operand;
    }
  else if (input_numbers_option_used (lo_input, hi_input))
    {
      if (n_operands)
        {
          error (0, 0, _("extra operand %s"), quote (operand[0]));
          usage (EXIT_FAILURE);
        }
      n_lines = hi_input - lo_input + 1;
      line = NULL;
    }
  else
    {
      switch (n_operands)
        {
        case 0:
          break;

        case 1:
          if (! (STREQ (operand[0], "-") || freopen (operand[0], "r", stdin)))
            error (EXIT_FAILURE, errno, "%s", operand[0]);
          break;

        default:
          error (0, 0, _("extra operand %s"), quote (operand[1]));
          usage (EXIT_FAILURE);
        }

      fadvise (stdin, FADVISE_SEQUENTIAL);

      n_lines = read_input (stdin, eolbyte, &input_lines);
      line = input_lines;
    }

  head_lines = MIN (head_lines, n_lines);

  randint_source = randint_all_new (random_source,
                                    randperm_bound (head_lines, n_lines));
  if (! randint_source)
    error (EXIT_FAILURE, errno, "%s", quotearg_colon (random_source));

  /* Close stdin now, rather than earlier, so that randint_all_new
     doesn't have to worry about opening something other than
     stdin.  */
  if (! (echo || input_numbers_option_used (lo_input, hi_input))
      && (fclose (stdin) != 0))
    error (EXIT_FAILURE, errno, _("read error"));

  permutation = randperm_new (randint_source, head_lines, n_lines);

  if (outfile && ! freopen (outfile, "w", stdout))
    error (EXIT_FAILURE, errno, "%s", quotearg_colon (outfile));
  if (write_permuted_output (head_lines, line, lo_input, permutation, eolbyte)
      != 0)
    error (EXIT_FAILURE, errno, _("write error"));

  free (permutation);
  randint_all_free (randint_source);
  if (input_lines)
    {
      free (input_lines[0]);
      free (input_lines);
    }

  return EXIT_SUCCESS;
}
