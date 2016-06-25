int
main (int argc, char **argv)
{
  char *hostname;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE_NAME, Version,
                      usage, AUTHORS, (char const *) NULL);
  if (getopt_long (argc, argv, "", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  if (argc == optind + 1)
    {
      /* Set hostname to operand.  */
      char const *name = argv[optind];
      if (sethostname (name, strlen (name)) != 0)
        error (EXIT_FAILURE, errno, _("cannot set name to %s"), quote (name));
      error (EXIT_FAILURE, 0,
             _("cannot set hostname; this system lacks the functionality"));
    }

  if (argc <= optind)
    {
      hostname = xgethostname ();
      if (hostname == NULL)
        error (EXIT_FAILURE, errno, _("cannot determine hostname"));
      printf ("%s\n", hostname);
    }

  if (optind + 1 < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
      usage (EXIT_FAILURE);
    }

  exit (EXIT_SUCCESS);
}
