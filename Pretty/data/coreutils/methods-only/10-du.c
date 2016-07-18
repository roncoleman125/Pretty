int
main (int argc, char **argv)
{
  char *cwd_only[2];
  bool max_depth_specified = false;
  bool ok = true;
  char *files_from = NULL;

  /* Bit flags that control how fts works.  */
  int bit_flags = FTS_NOSTAT;

  /* Select one of the three FTS_ options that control if/when
     to follow a symlink.  */
  int symlink_deref_bits = FTS_PHYSICAL;

  /* If true, display only a total for each argument. */
  bool opt_summarize_only = false;

  cwd_only[0] = bad_cast (".");
  cwd_only[1] = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  exclude = new_exclude ();

  human_options (getenv ("DU_BLOCK_SIZE"),
                 &human_output_opts, &output_block_size);

  while (true)
    {
      int oi = -1;
      int c = getopt_long (argc, argv, "0abd:chHklmsxB:DLPSX:",
                           long_options, &oi);
      if (c == -1)
        break;

      switch (c)
        {
        case FTS_DEBUG:
          fts_debug = true;
          break;

        case '0':
          opt_nul_terminate_output = true;
          break;

        case 'a':
          opt_all = true;
          break;

        case APPARENT_SIZE_OPTION:
          apparent_size = true;
          break;

        case 'b':
          apparent_size = true;
          human_output_opts = 0;
          output_block_size = 1;
          break;

        case 'c':
          print_grand_total = true;
          break;

        case 'h':
          human_output_opts = human_autoscale | human_SI | human_base_1024;
          output_block_size = 1;
          break;

        case HUMAN_SI_OPTION:
          human_output_opts = human_autoscale | human_SI;
          output_block_size = 1;
          break;

        case 'k':
          human_output_opts = 0;
          output_block_size = 1024;
          break;

        case 'd':		/* --max-depth=N */
          {
            unsigned long int tmp_ulong;
            if (xstrtoul (optarg, NULL, 0, &tmp_ulong, NULL) == LONGINT_OK
                && tmp_ulong <= SIZE_MAX)
              {
                max_depth_specified = true;
                max_depth = tmp_ulong;
              }
            else
              {
                error (0, 0, _("invalid maximum depth %s"),
                       quote (optarg));
                ok = false;
              }
          }
          break;

        case 'm':
          human_output_opts = 0;
          output_block_size = 1024 * 1024;
          break;

        case 'l':
          opt_count_all = true;
          break;

        case 's':
          opt_summarize_only = true;
          break;

        case 'x':
          bit_flags |= FTS_XDEV;
          break;

        case 'B':
          {
            enum strtol_error e = human_options (optarg, &human_output_opts,
                                                 &output_block_size);
            if (e != LONGINT_OK)
              xstrtol_fatal (e, oi, c, long_options, optarg);
          }
          break;

        case 'H':  /* NOTE: before 2008-12, -H was equivalent to --si.  */
        case 'D':
          symlink_deref_bits = FTS_COMFOLLOW | FTS_PHYSICAL;
          break;

        case 'L': /* --dereference */
          symlink_deref_bits = FTS_LOGICAL;
          break;

        case 'P': /* --no-dereference */
          symlink_deref_bits = FTS_PHYSICAL;
          break;

        case 'S':
          opt_separate_dirs = true;
          break;

        case 'X':
          if (add_exclude_file (add_exclude, exclude, optarg,
                                EXCLUDE_WILDCARDS, '\n'))
            {
              error (0, errno, "%s", quotearg_colon (optarg));
              ok = false;
            }
          break;

        case FILES0_FROM_OPTION:
          files_from = optarg;
          break;

        case EXCLUDE_OPTION:
          add_exclude (exclude, optarg, EXCLUDE_WILDCARDS);
          break;

        case TIME_OPTION:
          opt_time = true;
          time_type =
            (optarg
             ? XARGMATCH ("--time", optarg, time_args, time_types)
             : time_mtime);
          break;

        case TIME_STYLE_OPTION:
          time_style = optarg;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          ok = false;
        }
    }

  if (!ok)
    usage (EXIT_FAILURE);

  if (opt_all && opt_summarize_only)
    {
      error (0, 0, _("cannot both summarize and show all entries"));
      usage (EXIT_FAILURE);
    }

  if (opt_summarize_only && max_depth_specified && max_depth == 0)
    {
      error (0, 0,
             _("warning: summarizing is the same as using --max-depth=0"));
    }

  if (opt_summarize_only && max_depth_specified && max_depth != 0)
    {
      unsigned long int d = max_depth;
      error (0, 0, _("warning: summarizing conflicts with --max-depth=%lu"), d);
      usage (EXIT_FAILURE);
    }

  if (opt_summarize_only)
    max_depth = 0;

  /* Process time style if printing last times.  */
  if (opt_time)
    {
      if (! time_style)
        {
          time_style = getenv ("TIME_STYLE");

          /* Ignore TIMESTYLE="locale", for compatibility with ls.  */
          if (! time_style || STREQ (time_style, "locale"))
            time_style = "long-iso";
          else if (*time_style == '+')
            {
              /* Ignore anything after a newline, for compatibility
                 with ls.  */
              char *p = strchr (time_style, '\n');
              if (p)
                *p = '\0';
            }
          else
            {
              /* Ignore "posix-" prefix, for compatibility with ls.  */
              static char const posix_prefix[] = "posix-";
              while (strncmp (time_style, posix_prefix, sizeof posix_prefix - 1)
                     == 0)
                time_style += sizeof posix_prefix - 1;
            }
        }

      if (*time_style == '+')
        time_format = time_style + 1;
      else
        {
          switch (XARGMATCH ("time style", time_style,
                             time_style_args, time_style_types))
            {
            case full_iso_time_style:
              time_format = "%Y-%m-%d %H:%M:%S.%N %z";
              break;

            case long_iso_time_style:
              time_format = "%Y-%m-%d %H:%M";
              break;

            case iso_time_style:
              time_format = "%Y-%m-%d";
              break;
            }
        }
    }

  struct argv_iterator *ai;
  if (files_from)
    {
      /* When using --files0-from=F, you may not specify any files
         on the command-line.  */
      if (optind < argc)
        {
          error (0, 0, _("extra operand %s"), quote (argv[optind]));
          fprintf (stderr, "%s\n",
                   _("file operands cannot be combined with --files0-from"));
          usage (EXIT_FAILURE);
        }

      if (! (STREQ (files_from, "-") || freopen (files_from, "r", stdin)))
        error (EXIT_FAILURE, errno, _("cannot open %s for reading"),
               quote (files_from));

      ai = argv_iter_init_stream (stdin);

      /* It's not easy here to count the arguments, so assume the
         worst.  */
      hash_all = true;
    }
  else
    {
      char **files = (optind < argc ? argv + optind : cwd_only);
      ai = argv_iter_init_argv (files);

      /* Hash all dev,ino pairs if there are multiple arguments, or if
         following non-command-line symlinks, because in either case a
         file with just one hard link might be seen more than once.  */
      hash_all = (optind + 1 < argc || symlink_deref_bits == FTS_LOGICAL);
    }

  if (!ai)
    xalloc_die ();

  /* Initialize the set of dev,inode pairs.  */
  di_set = di_set_alloc ();
  if (!di_set)
    xalloc_die ();

  /* If not hashing everything, process_file won't find cycles on its
     own, so ask fts_read to check for them accurately.  */
  if (opt_count_all || ! hash_all)
    bit_flags |= FTS_TIGHT_CYCLE_CHECK;

  bit_flags |= symlink_deref_bits;
  static char *temp_argv[] = { NULL, NULL };

  while (true)
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
              error (0, errno, _("%s: read error"), quote (files_from));
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
             printf - | du --files0-from=- */
          error (0, 0, _("when reading file names from stdin, "
                         "no file name of %s allowed"),
                 quote (file_name));
          skip_file = true;
        }

      /* Report and skip any empty file names before invoking fts.
         This works around a glitch in fts, which fails immediately
         (without looking at the other file names) when given an empty
         file name.  */
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
        {
          temp_argv[0] = file_name;
          ok &= du_files (temp_argv, bit_flags);
        }
    }

  argv_iter_free (ai);
  di_set_free (di_set);

  if (files_from && (ferror (stdin) || fclose (stdin) != 0))
    error (EXIT_FAILURE, 0, _("error reading %s"), quote (files_from));

  if (print_grand_total)
    print_size (&tot_dui, _("total"));

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
