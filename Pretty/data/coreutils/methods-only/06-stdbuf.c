int
main (int argc, char **argv)
{
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (EXIT_CANCELED);
  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "+i:o:e:", longopts, NULL)) != -1)
    {
      int opt_fileno;

      switch (c)
        {
        /* Old McDonald had a farm ei...  */
        case 'e':
        case 'i':
        case 'o':
          opt_fileno = optc_to_fileno (c);
          assert (0 <= opt_fileno && opt_fileno < ARRAY_CARDINALITY (stdbuf));
          stdbuf[opt_fileno].optc = c;
          while (c_isspace (*optarg))
            optarg++;
          stdbuf[opt_fileno].optarg = optarg;
          if (c == 'i' && *optarg == 'L')
            {
              /* -oL will be by far the most common use of this utility,
                 but one could easily think -iL might have the same affect,
                 so disallow it as it could be confusing.  */
              error (0, 0, _("line buffering stdin is meaningless"));
              usage (EXIT_CANCELED);
            }

          if (!STREQ (optarg, "L")
              && parse_size (optarg, &stdbuf[opt_fileno].size) == -1)
            error (EXIT_CANCELED, errno, _("invalid mode %s"), quote (optarg));

          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_CANCELED);
        }
    }

  argv += optind;
  argc -= optind;

  /* must specify at least 1 command.  */
  if (argc < 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_CANCELED);
    }

  /* FIXME: Should we mandate at least one option?  */

  set_libstdbuf_options ();

  /* Try to preload libstdbuf first from the same path as
     stdbuf is running from.  */
  set_program_path (argv[0]);
  if (!program_path)
    program_path = xstrdup (PKGLIBDIR);  /* Need to init to non NULL.  */
  set_LD_PRELOAD ();
  free (program_path);

  execvp (*argv, argv);

  {
    int exit_status = (errno == ENOENT ? EXIT_ENOENT : EXIT_CANNOT_INVOKE);
    error (0, errno, _("failed to run command %s"), quote (argv[0]));
    exit (exit_status);
  }
}
