int
main (int argc, char **argv)
{
  char *format;
  int args_used;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  exit_status = EXIT_SUCCESS;

  posixly_correct = (getenv ("POSIXLY_CORRECT") != NULL);

  /* We directly parse options, rather than use parse_long_options, in
     order to avoid accepting abbreviations.  */
  if (argc == 2)
    {
      if (STREQ (argv[1], "--help"))
        usage (EXIT_SUCCESS);

      if (STREQ (argv[1], "--version"))
        {
          version_etc (stdout, PROGRAM_NAME, PACKAGE_NAME, Version, AUTHORS,
                       (char *) NULL);
          exit (EXIT_SUCCESS);
        }
    }

  /* The above handles --help and --version.
     Since there is no other invocation of getopt, handle `--' here.  */
  if (1 < argc && STREQ (argv[1], "--"))
    {
      --argc;
      ++argv;
    }

  if (argc <= 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  format = argv[1];
  argc -= 2;
  argv += 2;

  do
    {
      args_used = print_formatted (format, argc, argv);
      argc -= args_used;
      argv += args_used;
    }
  while (args_used > 0 && argc > 0);

  if (argc > 0)
    error (0, 0,
           _("warning: ignoring excess arguments, starting with %s"),
           quote (argv[0]));

  exit (exit_status);
}
