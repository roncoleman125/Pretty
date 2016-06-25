int
main (int argc, char **argv)
{
  bool ok;
  int optc;
  int files_given;
  bool (*sum_func) (const char *, int) = bsd_sum_file;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  /* Line buffer stdout to ensure lines are written atomically and immediately
     so that processes running in parallel do not intersperse their output.  */
  setvbuf (stdout, NULL, _IOLBF, 0);

  have_read_stdin = false;

  while ((optc = getopt_long (argc, argv, "rs", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'r':		/* For SysV compatibility. */
          sum_func = bsd_sum_file;
          break;

        case 's':
          sum_func = sysv_sum_file;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  files_given = argc - optind;
  if (files_given <= 0)
    ok = sum_func ("-", files_given);
  else
    for (ok = true; optind < argc; optind++)
      ok &= sum_func (argv[optind], files_given);

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, "-");
  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
