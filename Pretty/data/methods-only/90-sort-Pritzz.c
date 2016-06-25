int
main (int argc, char **argv)
{
  struct keyfield *key;
  struct keyfield key_buf;
  struct keyfield gkey;
  bool gkey_only = false;
  char const *s;
  int c = 0;
  char checkonly = 0;
  bool mergeonly = false;
  char *random_source = NULL;
  bool need_random = false;
  size_t nthreads = 0;
  size_t nfiles = 0;
  bool posixly_correct = (getenv ("POSIXLY_CORRECT") != NULL);
  bool obsolete_usage = (posix2_version () < 200112);
  char **files;
  char *files_from = NULL;
  struct Tokens tok;
  char const *outfile = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (SORT_FAILURE);

  hard_LC_COLLATE = hard_locale (LC_COLLATE);
  hard_LC_TIME = hard_locale (LC_TIME);

  /* Get locale's representation of the decimal point.  */
  {
    struct lconv const *locale = localeconv ();

    /* If the locale doesn't define a decimal point, or if the decimal
       point is multibyte, use the C locale's decimal point.  FIXME:
       add support for multibyte decimal points.  */
    decimal_point = to_uchar (locale->decimal_point[0]);
    if (! decimal_point || locale->decimal_point[1])
      decimal_point = '.';

    /* FIXME: add support for multibyte thousands separators.  */
    thousands_sep = to_uchar (*locale->thousands_sep);
    if (! thousands_sep || locale->thousands_sep[1])
      thousands_sep = -1;
  }

  have_read_stdin = false;
  inittables ();

  {
    size_t i;
    static int const sig[] =
      {
        /* The usual suspects.  */
        SIGALRM, SIGHUP, SIGINT, SIGPIPE, SIGQUIT, SIGTERM,
        SIGPOLL,
        SIGPROF,
        SIGVTALRM,
        SIGXCPU,
        SIGXFSZ,
      };
    enum { nsigs = ARRAY_CARDINALITY (sig) };

    struct sigaction act;

    sigemptyset (&caught_signals);
    for (i = 0; i < nsigs; i++)
      {
        sigaction (sig[i], NULL, &act);
        if (act.sa_handler != SIG_IGN)
          sigaddset (&caught_signals, sig[i]);
      }

    act.sa_handler = sighandler;
    act.sa_mask = caught_signals;
    act.sa_flags = 0;

    for (i = 0; i < nsigs; i++)
      if (sigismember (&caught_signals, sig[i]))
        sigaction (sig[i], &act, NULL);
    for (i = 0; i < nsigs; i++)
      if (signal (sig[i], SIG_IGN) != SIG_IGN)
        {
          signal (sig[i], sighandler);
          siginterrupt (sig[i], 1);
        }
  }
  signal (SIGCHLD, SIG_DFL); /* Don't inherit CHLD handling from parent.  */

  /* The signal mask is known, so it is safe to invoke exit_cleanup.  */
  atexit (exit_cleanup);

  key_init (&gkey);
  gkey.sword = SIZE_MAX;

  files = xnmalloc (argc, sizeof *files);

  while (true)
    {
      /* Parse an operand as a file after "--" was seen; or if
         pedantic and a file was seen, unless the POSIX version
         predates 1003.1-2001 and -c was not seen and the operand is
         "-o FILE" or "-oFILE".  */
      int oi = -1;

      if (c == -1
          || (posixly_correct && nfiles != 0
              && ! (obsolete_usage
                    && ! checkonly
                    && optind != argc
                    && argv[optind][0] == '-' && argv[optind][1] == 'o'
                    && (argv[optind][2] || optind + 1 != argc)))
          || ((c = getopt_long (argc, argv, short_options,
                                long_options, &oi))
              == -1))
        {
          if (argc <= optind)
            break;
          files[nfiles++] = argv[optind++];
        }
      else switch (c)
        {
        case 1:
          key = NULL;
          if (optarg[0] == '+')
            {
              bool minus_pos_usage = (optind != argc && argv[optind][0] == '-'
                                      && ISDIGIT (argv[optind][1]));
              obsolete_usage |= minus_pos_usage && !posixly_correct;
              if (obsolete_usage)
                {
                  /* Treat +POS1 [-POS2] as a key if possible; but silently
                     treat an operand as a file if it is not a valid +POS1.  */
                  key = key_init (&key_buf);
                  s = parse_field_count (optarg + 1, &key->sword, NULL);
                  if (s && *s == '.')
                    s = parse_field_count (s + 1, &key->schar, NULL);
                  if (! (key->sword || key->schar))
                    key->sword = SIZE_MAX;
                  if (! s || *set_ordering (s, key, bl_start))
                    key = NULL;
                  else
                    {
                      if (minus_pos_usage)
                        {
                          char const *optarg1 = argv[optind++];
                          s = parse_field_count (optarg1 + 1, &key->eword,
                                             N_("invalid number after `-'"));
                          if (*s == '.')
                            s = parse_field_count (s + 1, &key->echar,
                                               N_("invalid number after `.'"));
                          if (!key->echar && key->eword)
                            {
                              /* obsolescent syntax +A.x -B.y is equivalent to:
                                   -k A+1.x+1,B.y   (when y = 0)
                                   -k A+1.x+1,B+1.y (when y > 0)
                                 So eword is decremented as in the -k case
                                 only when the end field (B) is specified and
                                 echar (y) is 0.  */
                              key->eword--;
                            }
                          if (*set_ordering (s, key, bl_end))
                            badfieldspec (optarg1,
                                      N_("stray character in field spec"));
                        }
                      key->obsolete_used = true;
                      insertkey (key);
                    }
                }
            }
          if (! key)
            files[nfiles++] = optarg;
          break;

        case SORT_OPTION:
          c = XARGMATCH ("--sort", optarg, sort_args, sort_types);
          /* Fall through. */
        case 'b':
        case 'd':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'M':
        case 'n':
        case 'r':
        case 'R':
        case 'V':
          {
            char str[2];
            str[0] = c;
            str[1] = '\0';
            set_ordering (str, &gkey, bl_both);
          }
          break;

        case CHECK_OPTION:
          c = (optarg
               ? XARGMATCH ("--check", optarg, check_args, check_types)
               : 'c');
          /* Fall through.  */
        case 'c':
        case 'C':
          if (checkonly && checkonly != c)
            incompatible_options ("cC");
          checkonly = c;
          break;

        case COMPRESS_PROGRAM_OPTION:
          if (compress_program && !STREQ (compress_program, optarg))
            error (SORT_FAILURE, 0, _("multiple compress programs specified"));
          compress_program = optarg;
          break;

        case DEBUG_PROGRAM_OPTION:
          debug = true;
          break;

        case FILES0_FROM_OPTION:
          files_from = optarg;
          break;

        case 'k':
          key = key_init (&key_buf);

          /* Get POS1. */
          s = parse_field_count (optarg, &key->sword,
                                 N_("invalid number at field start"));
          if (! key->sword--)
            {
              /* Provoke with `sort -k0' */
              badfieldspec (optarg, N_("field number is zero"));
            }
          if (*s == '.')
            {
              s = parse_field_count (s + 1, &key->schar,
                                     N_("invalid number after `.'"));
              if (! key->schar--)
                {
                  /* Provoke with `sort -k1.0' */
                  badfieldspec (optarg, N_("character offset is zero"));
                }
            }
          if (! (key->sword || key->schar))
            key->sword = SIZE_MAX;
          s = set_ordering (s, key, bl_start);
          if (*s != ',')
            {
              key->eword = SIZE_MAX;
              key->echar = 0;
            }
          else
            {
              /* Get POS2. */
              s = parse_field_count (s + 1, &key->eword,
                                     N_("invalid number after `,'"));
              if (! key->eword--)
                {
                  /* Provoke with `sort -k1,0' */
                  badfieldspec (optarg, N_("field number is zero"));
                }
              if (*s == '.')
                {
                  s = parse_field_count (s + 1, &key->echar,
                                         N_("invalid number after `.'"));
                }
              s = set_ordering (s, key, bl_end);
            }
          if (*s)
            badfieldspec (optarg, N_("stray character in field spec"));
          insertkey (key);
          break;

        case 'm':
          mergeonly = true;
          break;

        case NMERGE_OPTION:
          specify_nmerge (oi, c, optarg);
          break;

        case 'o':
          if (outfile && !STREQ (outfile, optarg))
            error (SORT_FAILURE, 0, _("multiple output files specified"));
          outfile = optarg;
          break;

        case RANDOM_SOURCE_OPTION:
          if (random_source && !STREQ (random_source, optarg))
            error (SORT_FAILURE, 0, _("multiple random sources specified"));
          random_source = optarg;
          break;

        case 's':
          stable = true;
          break;

        case 'S':
          specify_sort_size (oi, c, optarg);
          break;

        case 't':
          {
            char newtab = optarg[0];
            if (! newtab)
              error (SORT_FAILURE, 0, _("empty tab"));
            if (optarg[1])
              {
                if (STREQ (optarg, "\\0"))
                  newtab = '\0';
                else
                  {
                    /* Provoke with `sort -txx'.  Complain about
                       "multi-character tab" instead of "multibyte tab", so
                       that the diagnostic's wording does not need to be
                       changed once multibyte characters are supported.  */
                    error (SORT_FAILURE, 0, _("multi-character tab %s"),
                           quote (optarg));
                  }
              }
            if (tab != TAB_DEFAULT && tab != newtab)
              error (SORT_FAILURE, 0, _("incompatible tabs"));
            tab = newtab;
          }
          break;

        case 'T':
          add_temp_dir (optarg);
          break;

        case PARALLEL_OPTION:
          nthreads = specify_nthreads (oi, c, optarg);
          break;

        case 'u':
          unique = true;
          break;

        case 'y':
          /* Accept and ignore e.g. -y0 for compatibility with Solaris 2.x
             through Solaris 7.  It is also accepted by many non-Solaris
             "sort" implementations, e.g., AIX 5.2, HP-UX 11i v2, IRIX 6.5.
             -y is marked as obsolete starting with Solaris 8 (1999), but is
             still accepted as of Solaris 10 prerelease (2004).

             Solaris 2.5.1 "sort -y 100" reads the input file "100", but
             emulate Solaris 8 and 9 "sort -y 100" which ignores the "100",
             and which in general ignores the argument after "-y" if it
             consists entirely of digits (it can even be empty).  */
          if (optarg == argv[optind - 1])
            {
              char const *p;
              for (p = optarg; ISDIGIT (*p); p++)
                continue;
              optind -= (*p != '\0');
            }
          break;

        case 'z':
          eolchar = 0;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (SORT_FAILURE);
        }
    }

  if (files_from)
    {
      FILE *stream;

      /* When using --files0-from=F, you may not specify any files
         on the command-line.  */
      if (nfiles)
        {
          error (0, 0, _("extra operand %s"), quote (files[0]));
          fprintf (stderr, "%s\n",
                   _("file operands cannot be combined with --files0-from"));
          usage (SORT_FAILURE);
        }

      if (STREQ (files_from, "-"))
        stream = stdin;
      else
        {
          stream = fopen (files_from, "r");
          if (stream == NULL)
            error (SORT_FAILURE, errno, _("cannot open %s for reading"),
                   quote (files_from));
        }

      readtokens0_init (&tok);

      if (! readtokens0 (stream, &tok) || fclose (stream) != 0)
        error (SORT_FAILURE, 0, _("cannot read file names from %s"),
               quote (files_from));

      if (tok.n_tok)
        {
          size_t i;
          free (files);
          files = tok.tok;
          nfiles = tok.n_tok;
          for (i = 0; i < nfiles; i++)
            {
              if (STREQ (files[i], "-"))
                error (SORT_FAILURE, 0, _("when reading file names from stdin, "
                                          "no file name of %s allowed"),
                       quote (files[i]));
              else if (files[i][0] == '\0')
                {
                  /* Using the standard `filename:line-number:' prefix here is
                     not totally appropriate, since NUL is the separator,
                     not NL, but it might be better than nothing.  */
                  unsigned long int file_number = i + 1;
                  error (SORT_FAILURE, 0,
                         _("%s:%lu: invalid zero-length file name"),
                         quotearg_colon (files_from), file_number);
                }
            }
        }
      else
        error (SORT_FAILURE, 0, _("no input from %s"),
               quote (files_from));
    }

  /* Inheritance of global options to individual keys. */
  for (key = keylist; key; key = key->next)
    {
      if (default_key_compare (key) && !key->reverse)
        {
          key->ignore = gkey.ignore;
          key->translate = gkey.translate;
          key->skipsblanks = gkey.skipsblanks;
          key->skipeblanks = gkey.skipeblanks;
          key->month = gkey.month;
          key->numeric = gkey.numeric;
          key->general_numeric = gkey.general_numeric;
          key->human_numeric = gkey.human_numeric;
          key->version = gkey.version;
          key->random = gkey.random;
          key->reverse = gkey.reverse;
        }

      need_random |= key->random;
    }

  if (!keylist && !default_key_compare (&gkey))
    {
      gkey_only = true;
      insertkey (&gkey);
      need_random |= gkey.random;
    }

  check_ordering_compatibility ();

  if (debug)
    {
      if (checkonly || outfile)
        {
          static char opts[] = "X --debug";
          opts[0] = (checkonly ? checkonly : 'o');
          incompatible_options (opts);
        }

      /* Always output the locale in debug mode, since this
         is such a common source of confusion.  */
      if (hard_LC_COLLATE)
        error (0, 0, _("using %s sorting rules"),
               quote (setlocale (LC_COLLATE, NULL)));
      else
        error (0, 0, _("using simple byte comparison"));
      key_warnings (&gkey, gkey_only);
    }

  reverse = gkey.reverse;

  if (need_random)
    random_md5_state_init (random_source);

  if (temp_dir_count == 0)
    {
      char const *tmp_dir = getenv ("TMPDIR");
      add_temp_dir (tmp_dir ? tmp_dir : DEFAULT_TMPDIR);
    }

  if (nfiles == 0)
    {
      static char *minus = (char *) "-";
      nfiles = 1;
      free (files);
      files = &minus;
    }

  /* Need to re-check that we meet the minimum requirement for memory
     usage with the final value for NMERGE. */
  if (0 < sort_size)
    sort_size = MAX (sort_size, MIN_SORT_SIZE);

  if (checkonly)
    {
      if (nfiles > 1)
        error (SORT_FAILURE, 0, _("extra operand %s not allowed with -%c"),
               quote (files[1]), checkonly);

      if (outfile)
        {
          static char opts[] = {0, 'o', 0};
          opts[0] = checkonly;
          incompatible_options (opts);
        }

      /* POSIX requires that sort return 1 IFF invoked with -c or -C and the
         input is not properly sorted.  */
      exit (check (files[0], checkonly) ? EXIT_SUCCESS : SORT_OUT_OF_ORDER);
    }

  if (mergeonly)
    {
      struct sortfile *sortfiles = xcalloc (nfiles, sizeof *sortfiles);
      size_t i;

      for (i = 0; i < nfiles; ++i)
        sortfiles[i].name = files[i];

      merge (sortfiles, 0, nfiles, outfile);
      IF_LINT (free (sortfiles));
    }
  else
    {
      if (!nthreads)
        {
          unsigned long int np = num_processors (NPROC_CURRENT_OVERRIDABLE);
          nthreads = MIN (np, DEFAULT_MAX_THREADS);
        }

      /* Avoid integer overflow later.  */
      size_t nthreads_max = SIZE_MAX / (2 * sizeof (struct merge_node));
      nthreads = MIN (nthreads, nthreads_max);

      sort (files, nfiles, outfile, nthreads);
    }

  if (have_read_stdin && fclose (stdin) == EOF)
    die (_("close failed"), "-");

  exit (EXIT_SUCCESS);
}
