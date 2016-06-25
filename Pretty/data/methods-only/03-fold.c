int
main (int argc, char **argv)
{
  size_t width = 80;
  int i;
  int optc;
  bool ok;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  break_spaces = count_bytes = have_read_stdin = false;

  while ((optc = getopt_long (argc, argv, shortopts, longopts, NULL)) != -1)
    {
      char optargbuf[2];

      switch (optc)
        {
        case 'b':		/* Count bytes rather than columns. */
          count_bytes = true;
          break;

        case 's':		/* Break at word boundaries. */
          break_spaces = true;
          break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          if (optarg)
            optarg--;
          else
            {
              optargbuf[0] = optc;
              optargbuf[1] = '\0';
              optarg = optargbuf;
            }
          /* Fall through.  */
        case 'w':		/* Line width. */
          {
            unsigned long int tmp_ulong;
            if (! (xstrtoul (optarg, NULL, 10, &tmp_ulong, "") == LONGINT_OK
                   && 0 < tmp_ulong && tmp_ulong < SIZE_MAX - TAB_WIDTH))
              error (EXIT_FAILURE, 0,
                     _("invalid number of columns: %s"), quote (optarg));
            width = tmp_ulong;
          }
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (argc == optind)
    ok = fold_file ("-", width);
  else
    {
      ok = true;
      for (i = optind; i < argc; i++)
        ok &= fold_file (argv[i], width);
    }

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, "-");

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
