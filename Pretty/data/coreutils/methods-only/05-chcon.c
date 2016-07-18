int
main (int argc, char **argv)
{
  security_context_t ref_context = NULL;

  /* Bit flags that control how fts works.  */
  int bit_flags = FTS_PHYSICAL;

  /* 1 if --dereference, 0 if --no-dereference, -1 if neither has been
     specified.  */
  int dereference = -1;

  bool ok;
  bool preserve_root = false;
  bool component_specified = false;
  char *reference_file = NULL;
  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "HLPRhvu:r:t:l:", long_options, NULL))
         != -1)
    {
      switch (optc)
        {
        case 'H': /* Traverse command-line symlinks-to-directories.  */
          bit_flags = FTS_COMFOLLOW | FTS_PHYSICAL;
          break;

        case 'L': /* Traverse all symlinks-to-directories.  */
          bit_flags = FTS_LOGICAL;
          break;

        case 'P': /* Traverse no symlinks-to-directories.  */
          bit_flags = FTS_PHYSICAL;
          break;

        case 'h': /* --no-dereference: affect symlinks */
          dereference = 0;
          break;

        case DEREFERENCE_OPTION: /* --dereference: affect the referent
                                    of each symlink */
          dereference = 1;
          break;

        case NO_PRESERVE_ROOT:
          preserve_root = false;
          break;

        case PRESERVE_ROOT:
          preserve_root = true;
          break;

        case REFERENCE_FILE_OPTION:
          reference_file = optarg;
          break;

        case 'R':
          recurse = true;
          break;

        case 'f':
          /* ignore */
          break;

        case 'v':
          verbose = true;
          break;

        case 'u':
          specified_user = optarg;
          component_specified = true;
          break;

        case 'r':
          specified_role = optarg;
          component_specified = true;
          break;

        case 't':
          specified_type = optarg;
          component_specified = true;
          break;

        case 'l':
          specified_range = optarg;
          component_specified = true;
          break;

        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (recurse)
    {
      if (bit_flags == FTS_PHYSICAL)
        {
          if (dereference == 1)
            error (EXIT_FAILURE, 0,
                   _("-R --dereference requires either -H or -L"));
          affect_symlink_referent = false;
        }
      else
        {
          if (dereference == 0)
            error (EXIT_FAILURE, 0, _("-R -h requires -P"));
          affect_symlink_referent = true;
        }
    }
  else
    {
      bit_flags = FTS_PHYSICAL;
      affect_symlink_referent = (dereference != 0);
    }

  if (argc - optind < (reference_file || component_specified ? 1 : 2))
    {
      if (argc <= optind)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      usage (EXIT_FAILURE);
    }

  if (reference_file)
    {
      if (getfilecon (reference_file, &ref_context) < 0)
        error (EXIT_FAILURE, errno, _("failed to get security context of %s"),
               quote (reference_file));

      specified_context = ref_context;
    }
  else if (component_specified)
    {
      /* FIXME: it's already null, so this is a no-op. */
      specified_context = NULL;
    }
  else
    {
      context_t context;
      specified_context = argv[optind++];
      context = context_new (specified_context);
      if (!context)
        error (EXIT_FAILURE, 0, _("invalid context: %s"),
               quotearg_colon (specified_context));
      context_free (context);
    }

  if (reference_file && component_specified)
    {
      error (0, 0, _("conflicting security context specifiers given"));
      usage (EXIT_FAILURE);
    }

  if (recurse && preserve_root)
    {
      static struct dev_ino dev_ino_buf;
      root_dev_ino = get_root_dev_ino (&dev_ino_buf);
      if (root_dev_ino == NULL)
        error (EXIT_FAILURE, errno, _("failed to get attributes of %s"),
               quote ("/"));
    }
  else
    {
      root_dev_ino = NULL;
    }

  ok = process_files (argv + optind, bit_flags | FTS_NOSTAT);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
