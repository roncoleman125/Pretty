/* Parse the ASCII mode given on the command line into a linked list
   of `struct mode_change' and apply that to each file argument. */

int
main (int argc, char **argv)
{
  char *mode = NULL;
  size_t mode_len = 0;
  size_t mode_alloc = 0;
  bool ok;
  bool preserve_root = false;
  char const *reference_file = NULL;
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  recurse = force_silent = diagnose_surprises = false;

  while ((c = getopt_long (argc, argv,
                           "Rcfvr::w::x::X::s::t::u::g::o::a::,::+::=::",
                           long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case 'r':
        case 'w':
        case 'x':
        case 'X':
        case 's':
        case 't':
        case 'u':
        case 'g':
        case 'o':
        case 'a':
        case ',':
        case '+':
        case '=':
          /* Support nonportable uses like "chmod -w", but diagnose
             surprises due to umask confusion.  Even though "--", "--r",
             etc., are valid modes, there is no "case '-'" here since
             getopt_long reserves leading "--" for long options.  */
          {
            /* Allocate a mode string (e.g., "-rwx") by concatenating
               the argument containing this option.  If a previous mode
               string was given, concatenate the previous string, a
               comma, and the new string (e.g., "-s,-rwx").  */

            char const *arg = argv[optind - 1];
            size_t arg_len = strlen (arg);
            size_t mode_comma_len = mode_len + !!mode_len;
            size_t new_mode_len = mode_comma_len + arg_len;
            if (mode_alloc <= new_mode_len)
              {
                mode_alloc = new_mode_len + 1;
                mode = X2REALLOC (mode, &mode_alloc);
              }
            mode[mode_len] = ',';
            strcpy (mode + mode_comma_len, arg);
            mode_len = new_mode_len;

            diagnose_surprises = true;
          }
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
        case 'c':
          verbosity = V_changes_only;
          break;
        case 'f':
          force_silent = true;
          break;
        case 'v':
          verbosity = V_high;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (reference_file)
    {
      if (mode)
        {
          error (0, 0, _("cannot combine mode and --reference options"));
          usage (EXIT_FAILURE);
        }
    }
  else
    {
      if (!mode)
        mode = argv[optind++];
    }

  if (optind >= argc)
    {
      if (!mode || mode != argv[optind - 1])
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      usage (EXIT_FAILURE);
    }

  if (reference_file)
    {
      change = mode_create_from_ref (reference_file);
      if (!change)
        error (EXIT_FAILURE, errno, _("failed to get attributes of %s"),
               quote (reference_file));
    }
  else
    {
      change = mode_compile (mode);
      if (!change)
        {
          error (0, 0, _("invalid mode: %s"), quote (mode));
          usage (EXIT_FAILURE);
        }
      umask_value = umask (0);
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

  ok = process_files (argv + optind,
                      FTS_COMFOLLOW | FTS_PHYSICAL | FTS_DEFER_STAT);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
