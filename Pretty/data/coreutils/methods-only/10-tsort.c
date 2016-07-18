int
main (int argc, char **argv)
{
  bool ok;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE, Version,
                      usage, AUTHORS, (char const *) NULL);
  if (getopt_long (argc, argv, "", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  if (1 < argc - optind)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
      usage (EXIT_FAILURE);
    }

  ok = tsort (optind == argc ? "-" : argv[optind]);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
