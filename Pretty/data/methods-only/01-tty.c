int
main (int argc, char **argv)
{
  char *tty;
  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (TTY_WRITE_ERROR);
  atexit (close_stdout);

  silent = false;

  while ((optc = getopt_long (argc, argv, "s", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 's':
          silent = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (TTY_FAILURE);
        }
    }

  if (optind < argc)
    error (0, 0, _("extra operand %s"), quote (argv[optind]));

  tty = ttyname (STDIN_FILENO);
  if (!silent)
    {
      if (tty)
        puts (tty);
      else
        puts (_("not a tty"));
    }

  exit (isatty (STDIN_FILENO) ? EXIT_SUCCESS : EXIT_FAILURE);
}
