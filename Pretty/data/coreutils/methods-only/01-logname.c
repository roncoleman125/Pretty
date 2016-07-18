int
main (int argc, char **argv)
{
  char *cp;

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

  if (optind < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  /* POSIX requires using getlogin (or equivalent code).  */
  cp = getlogin ();
  if (cp)
    {
      puts (cp);
      exit (EXIT_SUCCESS);
    }
  /* POSIX prohibits using a fallback technique.  */

  error (0, 0, _("no login name"));
  exit (EXIT_FAILURE);
}
