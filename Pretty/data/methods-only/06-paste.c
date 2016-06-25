int
main (int argc, char **argv)
{
  int optc;
  bool ok;
  char const *delim_arg = "\t";

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;
  serial_merge = false;

  while ((optc = getopt_long (argc, argv, "d:s", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'd':
          /* Delimiter character(s). */
          delim_arg = (optarg[0] == '\0' ? "\\0" : optarg);
          break;

        case 's':
          serial_merge = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (optind == argc)
    argv[argc++] = bad_cast ("-");

  if (collapse_escapes (delim_arg))
    {
      /* Don't use the default quoting style, because that would double the
         number of displayed backslashes, making the diagnostic look bogus.  */
      set_quoting_style (NULL, escape_quoting_style);
      error (EXIT_FAILURE, 0,
             _("delimiter list ends with an unescaped backslash: %s"),
             quotearg_colon (delim_arg));
    }

  if (!serial_merge)
    ok = paste_parallel (argc - optind, &argv[optind]);
  else
    ok = paste_serial (argc - optind, &argv[optind]);

  free (delims);

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, "-");
  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
