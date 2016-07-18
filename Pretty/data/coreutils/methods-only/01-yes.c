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
  if (getopt_long (argc, argv, "+", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  if (argc <= optind)
    {
      optind = argc;
      argv[argc++] = bad_cast ("y");
    }

  while (true)
    {
      int i;
      for (i = optind; i < argc; i++)
        if (fputs (argv[i], stdout) == EOF
            || putchar (i == argc - 1 ? '\n' : ' ') == EOF)
          {
            error (0, errno, _("standard output"));
            exit (EXIT_FAILURE);
          }
    }
}
