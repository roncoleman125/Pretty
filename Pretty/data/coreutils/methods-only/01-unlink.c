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

  if (argc < optind + 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  if (optind + 1 < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
      usage (EXIT_FAILURE);
    }

  if (unlink (argv[optind]) != 0)
    error (EXIT_FAILURE, errno, _("cannot unlink %s"), quote (argv[optind]));

  exit (EXIT_SUCCESS);
}
