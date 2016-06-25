int
main (int argc, char **argv)
{
  bool ok;
  int optc;
  int nfiles;
  char **files;
  char *files_from = NULL;
  struct fstatus *fstatus;
  struct Tokens tok;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  /* Line buffer stdout to ensure lines are written atomically and immediately
     so that processes running in parallel do not intersperse their output.  */
  setvbuf (stdout, NULL, _IOLBF, 0);

  print_lines = print_words = print_chars = print_bytes = false;
  print_linelength = false;
  total_lines = total_words = total_chars = total_bytes = max_line_length = 0;

  while ((optc = getopt_long (argc, argv, "clLmw", longopts, NULL)) != -1)
    switch (optc)
      {
      case 'c':
        print_bytes = true;
        break;

      case 'm':
        print_chars = true;
        break;

      case 'l':
        print_lines = true;
        break;

      case 'w':
        print_words = true;
        break;

      case 'L':
        print_linelength = true;
        break;

      case FILES0_FROM_OPTION:
        files_from = optarg;
        break;

      case_GETOPT_HELP_CHAR;

      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

      default:
        usage (EXIT_FAILURE);
      }

  if (! (print_lines || print_words || print_chars || print_bytes
         || print_linelength))
    print_lines = print_words = print_bytes = true;

  bool read_tokens = false;
  struct argv_iterator *ai;
  if (files_from)
    {
      FILE *stream;

      /* When using --files0-from=F, you may not specify any files
         on the command-line.  */
      if (optind < argc)
        {
          error (0, 0, _("extra operand %s"), quote (argv[optind]));
          fprintf (stderr, "%s\n",
                   _("file operands cannot be combined with --files0-from"));
          usage (EXIT_FAILURE);
        }

      if (STREQ (files_from, "-"))
        stream = stdin;
      else
        {
          stream = fopen (files_from, "r");
          if (stream == NULL)
            error (EXIT_FAILURE, errno, _("cannot open %s for reading"),
                   quote (files_from));
        }

      /* Read the file list into RAM if we can detect its size and that
         size is reasonable.  Otherwise, we'll read a name at a time.  */
      struct stat st;
      if (fstat (fileno (stream), &st) == 0
          && S_ISREG (st.st_mode)
          && st.st_size <= MIN (10 * 1024 * 1024, physmem_available () / 2))
        {
          read_tokens = true;
          readtokens0_init (&tok);
          if (! readtokens0 (stream, &tok) || fclose (stream) != 0)
            error (EXIT_FAILURE, 0, _("cannot read file names from %s"),
                   quote (files_from));
          files = tok.tok;
          nfiles = tok.n_tok;
          ai = argv_iter_init_argv (files);
        }
      else
        {
          files = NULL;
          nfiles = 0;
          ai = argv_iter_init_stream (stream);
        }
    }
  else
    {
      static char *stdin_only[] = { NULL };
      files = (optind < argc ? argv + optind : stdin_only);
      nfiles = (optind < argc ? argc - optind : 1);
      ai = argv_iter_init_argv (files);
    }

  fstatus = get_input_fstatus (nfiles, files);
  number_width = compute_number_width (nfiles, fstatus);

  int i;
  ok = true;
  for (i = 0; /* */; i++)
    {
      bool skip_file = false;
      enum argv_iter_err ai_err;
      char *file_name = argv_iter (ai, &ai_err);
      if (ai_err == AI_ERR_EOF)
        break;
      if (!file_name)
        {
          switch (ai_err)
            {
            case AI_ERR_READ:
              error (EXIT_FAILURE, errno, _("%s: read error"),
                     quote (files_from));
              continue;
            case AI_ERR_MEM:
              xalloc_die ();
            default:
              assert (!"unexpected error code from argv_iter");
            }
        }
      if (files_from && STREQ (files_from, "-") && STREQ (file_name, "-"))
        {
          /* Give a better diagnostic in an unusual case:
             printf - | wc --files0-from=- */
          error (0, 0, _("when reading file names from stdin, "
                         "no file name of %s allowed"),
                 quote (file_name));
          skip_file = true;
        }

      if (!file_name[0])
        {
          /* Diagnose a zero-length file name.  When it's one
             among many, knowing the record number may help.
             FIXME: currently print the record number only with
             --files0-from=FILE.  Maybe do it for argv, too?  */
          if (files_from == NULL)
            error (0, 0, "%s", _("invalid zero-length file name"));
          else
            {
              /* Using the standard `filename:line-number:' prefix here is
                 not totally appropriate, since NUL is the separator, not NL,
                 but it might be better than nothing.  */
              unsigned long int file_number = argv_iter_n_args (ai);
              error (0, 0, "%s:%lu: %s", quotearg_colon (files_from),
                     file_number, _("invalid zero-length file name"));
            }
          skip_file = true;
        }

      if (skip_file)
        ok = false;
      else
        ok &= wc_file (file_name, &fstatus[nfiles ? i : 0]);
    }

  /* No arguments on the command line is fine.  That means read from stdin.
     However, no arguments on the --files0-from input stream is an error
     means don't read anything.  */
  if (ok && !files_from && argv_iter_n_args (ai) == 0)
    ok &= wc_file (NULL, &fstatus[0]);

  if (read_tokens)
    readtokens0_free (&tok);

  if (1 < argv_iter_n_args (ai))
    write_counts (total_lines, total_words, total_chars, total_bytes,
                  max_line_length, _("total"));

  argv_iter_free (ai);

  free (fstatus);

  if (have_read_stdin && close (STDIN_FILENO) != 0)
    error (EXIT_FAILURE, errno, "-");

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
