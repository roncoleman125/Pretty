int
main (int argc, char **argv)
{
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

  if (argc < optind + 2)
    {
      if (argc < optind + 1)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  if (optind + 2 < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 2]));
      usage (EXIT_FAILURE);
    }

  if (link (argv[optind], argv[optind + 1]) != 0)
    error (EXIT_FAILURE, errno, _("cannot create link %s to %s"),
           quote_n (0, argv[optind + 1]), quote_n (1, argv[optind]));

  exit (EXIT_SUCCESS);
}
