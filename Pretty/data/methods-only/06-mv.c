int
main (int argc, char **argv)
{
  int c;
  bool ok;
  bool make_backups = false;
  char *backup_suffix_string;
  char *version_control_string = NULL;
  struct cp_options x;
  char *target_directory = NULL;
  bool no_target_directory = false;
  int n_files;
  char **file;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdin);

  cp_option_init (&x);

  /* Try to disable the ability to unlink a directory.  */
  priv_set_remove_linkdir ();

  /* FIXME: consider not calling getenv for SIMPLE_BACKUP_SUFFIX unless
     we'll actually use backup_suffix_string.  */
  backup_suffix_string = getenv ("SIMPLE_BACKUP_SUFFIX");

  while ((c = getopt_long (argc, argv, "bfint:uvS:T", long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case 'b':
          make_backups = true;
          if (optarg)
            version_control_string = optarg;
          break;
        case 'f':
          x.interactive = I_ALWAYS_YES;
          break;
        case 'i':
          x.interactive = I_ASK_USER;
          break;
        case 'n':
          x.interactive = I_ALWAYS_NO;
          break;
        case STRIP_TRAILING_SLASHES_OPTION:
          remove_trailing_slashes = true;
          break;
        case 't':
          if (target_directory)
            error (EXIT_FAILURE, 0, _("multiple target directories specified"));
          else
            {
              struct stat st;
              if (stat (optarg, &st) != 0)
                error (EXIT_FAILURE, errno, _("accessing %s"), quote (optarg));
              if (! S_ISDIR (st.st_mode))
                error (EXIT_FAILURE, 0, _("target %s is not a directory"),
                       quote (optarg));
            }
          target_directory = optarg;
          break;
        case 'T':
          no_target_directory = true;
          break;
        case 'u':
          x.update = true;
          break;
        case 'v':
          x.verbose = true;
          break;
        case 'S':
          make_backups = true;
          backup_suffix_string = optarg;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  n_files = argc - optind;
  file = argv + optind;

  if (n_files <= !target_directory)
    {
      if (n_files <= 0)
        error (0, 0, _("missing file operand"));
      else
        error (0, 0, _("missing destination file operand after %s"),
               quote (file[0]));
      usage (EXIT_FAILURE);
    }

  if (no_target_directory)
    {
      if (target_directory)
        error (EXIT_FAILURE, 0,
               _("cannot combine --target-directory (-t) "
                 "and --no-target-directory (-T)"));
      if (2 < n_files)
        {
          error (0, 0, _("extra operand %s"), quote (file[2]));
          usage (EXIT_FAILURE);
        }
    }
  else if (!target_directory)
    {
      assert (2 <= n_files);
      if (target_directory_operand (file[n_files - 1]))
        target_directory = file[--n_files];
      else if (2 < n_files)
        error (EXIT_FAILURE, 0, _("target %s is not a directory"),
               quote (file[n_files - 1]));
    }

  if (make_backups && x.interactive == I_ALWAYS_NO)
    {
      error (0, 0,
             _("options --backup and --no-clobber are mutually exclusive"));
      usage (EXIT_FAILURE);
    }

  if (backup_suffix_string)
    simple_backup_suffix = xstrdup (backup_suffix_string);

  x.backup_type = (make_backups
                   ? xget_version (_("backup type"),
                                   version_control_string)
                   : no_backups);

  hash_init ();

  if (target_directory)
    {
      int i;

      /* Initialize the hash table only if we'll need it.
         The problem it is used to detect can arise only if there are
         two or more files to move.  */
      if (2 <= n_files)
        dest_info_init (&x);

      ok = true;
      for (i = 0; i < n_files; ++i)
        ok &= movefile (file[i], target_directory, true, &x);
    }
  else
    ok = movefile (file[0], file[1], false, &x);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
