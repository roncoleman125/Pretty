int
main (int argc, char **argv)
{
  bool ok = true;
  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  remove_empty_parents = false;

  while ((optc = getopt_long (argc, argv, "pv", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'p':
          remove_empty_parents = true;
          break;
        case IGNORE_FAIL_ON_NON_EMPTY_OPTION:
          ignore_fail_on_non_empty = true;
          break;
        case 'v':
          verbose = true;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (optind == argc)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  for (; optind < argc; ++optind)
    {
      char *dir = argv[optind];

      /* Give a diagnostic for each attempted removal if --verbose.  */
      if (verbose)
        prog_fprintf (stdout, _("removing directory, %s"), quote (dir));

      if (rmdir (dir) != 0)
        {
          if (ignorable_failure (errno, dir))
            continue;

          /* Here, the diagnostic is less precise, since we have no idea
             whether DIR is a directory.  */
          error (0, errno, _("failed to remove %s"), quote (dir));
          ok = false;
        }
      else if (remove_empty_parents)
        {
          ok &= remove_parents (dir);
        }
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
