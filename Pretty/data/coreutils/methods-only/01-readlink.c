int
main (int argc, char **argv)
{
  /* If not -1, use this method to canonicalize.  */
  int can_mode = -1;

  /* File name to canonicalize.  */
  const char *fname;

  /* Result of canonicalize.  */
  char *value;

  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "efmnqsv", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'e':
          can_mode = CAN_EXISTING;
          break;
        case 'f':
          can_mode = CAN_ALL_BUT_LAST;
          break;
        case 'm':
          can_mode = CAN_MISSING;
          break;
        case 'n':
          no_newline = true;
          break;
        case 'q':
        case 's':
          verbose = false;
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

  if (optind >= argc)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  fname = argv[optind++];

  if (optind < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  value = (can_mode != -1
           ? canonicalize_filename_mode (fname, can_mode)
           : areadlink_with_size (fname, 63));
  if (value)
    {
      printf ("%s%s", value, (no_newline ? "" : "\n"));
      free (value);
      return EXIT_SUCCESS;
    }

  if (verbose)
    error (EXIT_FAILURE, errno, "%s", fname);

  return EXIT_FAILURE;
}
