int
main (int argc, char **argv)
{
  int opt;
  FILE *input_fh;
  const char *infile;

  /* True if --decode has been given and we should decode data. */
  bool decode = false;
  /* True if we should ignore non-base64-alphabetic characters. */
  bool ignore_garbage = false;
  /* Wrap encoded base64 data around the 76:th column, by default. */
  uintmax_t wrap_column = 76;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((opt = getopt_long (argc, argv, "diw:", long_options, NULL)) != -1)
    switch (opt)
      {
      case 'd':
        decode = true;
        break;

      case 'w':
        if (xstrtoumax (optarg, NULL, 0, &wrap_column, NULL) != LONGINT_OK)
          error (EXIT_FAILURE, 0, _("invalid wrap size: %s"),
                 quotearg (optarg));
        break;

      case 'i':
        ignore_garbage = true;
        break;

      case_GETOPT_HELP_CHAR;

      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

      default:
        usage (EXIT_FAILURE);
        break;
      }

  if (argc - optind > 1)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  if (optind < argc)
    infile = argv[optind];
  else
    infile = "-";

  if (STREQ (infile, "-"))
    {
      if (O_BINARY)
        xfreopen (NULL, "rb", stdin);
      input_fh = stdin;
    }
  else
    {
      input_fh = fopen (infile, "rb");
      if (input_fh == NULL)
        error (EXIT_FAILURE, errno, "%s", infile);
    }

  fadvise (input_fh, FADVISE_SEQUENTIAL);

  if (decode)
    do_decode (input_fh, stdout, ignore_garbage);
  else
    do_encode (input_fh, stdout, wrap_column);

  if (fclose (input_fh) == EOF)
    {
      if (STREQ (infile, "-"))
        error (EXIT_FAILURE, errno, _("closing standard input"));
      else
        error (EXIT_FAILURE, errno, "%s", infile);
    }

  exit (EXIT_SUCCESS);
}
