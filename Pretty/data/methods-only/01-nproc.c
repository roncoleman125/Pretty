int
main (int argc, char **argv)
{
  unsigned long nproc, ignore = 0;
  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  enum nproc_query mode = NPROC_CURRENT_OVERRIDABLE;

  while (1)
    {
      int c = getopt_long (argc, argv, "", longopts, NULL);
      if (c == -1)
        break;
      switch (c)
        {
        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        case ALL_OPTION:
          mode = NPROC_ALL;
          break;

        case IGNORE_OPTION:
          if (xstrtoul (optarg, NULL, 10, &ignore, "") != LONGINT_OK)
            {
              error (0, 0, _("%s: invalid number to ignore"), optarg);
              usage (EXIT_FAILURE);
            }
          break;

        default:
          usage (EXIT_FAILURE);
        }
    }

  nproc = num_processors (mode);

  if (ignore < nproc)
    nproc -= ignore;
  else
    nproc = 1;

  printf ("%lu\n", nproc);

  exit (EXIT_SUCCESS);
}
