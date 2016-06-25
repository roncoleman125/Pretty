int
main (int argc, char **argv)
{
  bool ok;
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "", long_options, NULL)) != -1)
    {
      switch (c)
        {
        case VERBOSE_OPTION:
          verbose = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (argc <= optind)
    ok = do_stdin ();
  else
    {
      int i;
      ok = true;
      for (i = optind; i < argc; i++)
        if (! print_factors (argv[i]))
          ok = false;
    }
  free (factor);
  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
