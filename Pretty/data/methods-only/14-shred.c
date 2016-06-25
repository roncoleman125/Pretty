int
main (int argc, char **argv)
{
  bool ok = true;
  DECLARE_ZEROED_AGGREGATE (struct Options, flags);
  char **file;
  int n_files;
  int c;
  int i;
  char const *random_source = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  flags.n_iterations = DEFAULT_PASSES;
  flags.size = -1;

  while ((c = getopt_long (argc, argv, "fn:s:uvxz", long_opts, NULL)) != -1)
    {
      switch (c)
        {
        case 'f':
          flags.force = true;
          break;

        case 'n':
          {
            uintmax_t tmp;
            if (xstrtoumax (optarg, NULL, 10, &tmp, NULL) != LONGINT_OK
                || MIN (UINT32_MAX, SIZE_MAX / sizeof (int)) < tmp)
              {
                error (EXIT_FAILURE, 0, _("%s: invalid number of passes"),
                       quotearg_colon (optarg));
              }
            flags.n_iterations = tmp;
          }
          break;

        case RANDOM_SOURCE_OPTION:
          if (random_source && !STREQ (random_source, optarg))
            error (EXIT_FAILURE, 0, _("multiple random sources specified"));
          random_source = optarg;
          break;

        case 'u':
          flags.remove_file = true;
          break;

        case 's':
          {
            uintmax_t tmp;
            if (xstrtoumax (optarg, NULL, 0, &tmp, "cbBkKMGTPEZY0")
                != LONGINT_OK)
              {
                error (EXIT_FAILURE, 0, _("%s: invalid file size"),
                       quotearg_colon (optarg));
              }
            flags.size = tmp;
          }
          break;

        case 'v':
          flags.verbose = true;
          break;

        case 'x':
          flags.exact = true;
          break;

        case 'z':
          flags.zero_fill = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  file = argv + optind;
  n_files = argc - optind;

  if (n_files == 0)
    {
      error (0, 0, _("missing file operand"));
      usage (EXIT_FAILURE);
    }

  randint_source = randint_all_new (random_source, SIZE_MAX);
  if (! randint_source)
    error (EXIT_FAILURE, errno, "%s", quotearg_colon (random_source));
  atexit (clear_random_data);

  for (i = 0; i < n_files; i++)
    {
      char *qname = xstrdup (quotearg_colon (file[i]));
      if (STREQ (file[i], "-"))
        {
          ok &= wipefd (STDOUT_FILENO, qname, randint_source, &flags);
        }
      else
        {
          /* Plain filename - Note that this overwrites *argv! */
          ok &= wipefile (file[i], qname, randint_source, &flags);
        }
      free (qname);
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
/*
 * vim:sw=2:sts=2:
 */
