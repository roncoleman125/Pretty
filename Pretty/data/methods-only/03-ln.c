int
main (int argc, char **argv)
{
  int c;
  bool ok;
  bool make_backups = false;
  char *backup_suffix_string;
  char *version_control_string = NULL;
  char const *target_directory = NULL;
  bool no_target_directory = false;
  int n_files;
  char **file;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdin);

  /* FIXME: consider not calling getenv for SIMPLE_BACKUP_SUFFIX unless
     we'll actually use backup_suffix_string.  */
  backup_suffix_string = getenv ("SIMPLE_BACKUP_SUFFIX");

  symbolic_link = remove_existing_files = interactive = verbose
    = hard_dir_link = false;

  while ((c = getopt_long (argc, argv, "bdfinst:vFLPS:T", long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case 'b':
          make_backups = true;
          if (optarg)
            version_control_string = optarg;
          break;
        case 'd':
        case 'F':
          hard_dir_link = true;
          break;
        case 'f':
          remove_existing_files = true;
          interactive = false;
          break;
        case 'i':
          remove_existing_files = false;
          interactive = true;
          break;
        case 'L':
          logical = true;
          break;
        case 'n':
          dereference_dest_dir_symlinks = false;
          break;
        case 'P':
          logical = false;
          break;
        case 's':
          symbolic_link = true;
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
        case 'v':
          verbose = true;
          break;
        case 'S':
          make_backups = true;
          backup_suffix_string = optarg;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
          break;
        }
    }

  n_files = argc - optind;
  file = argv + optind;

  if (n_files <= 0)
    {
      error (0, 0, _("missing file operand"));
      usage (EXIT_FAILURE);
    }

  if (no_target_directory)
    {
      if (target_directory)
        error (EXIT_FAILURE, 0,
               _("cannot combine --target-directory "
                 "and --no-target-directory"));
      if (n_files != 2)
        {
          if (n_files < 2)
            error (0, 0,
                   _("missing destination file operand after %s"),
                   quote (file[0]));
          else
            error (0, 0, _("extra operand %s"), quote (file[2]));
          usage (EXIT_FAILURE);
        }
    }
  else if (!target_directory)
    {
      if (n_files < 2)
        target_directory = ".";
      else if (2 <= n_files && target_directory_operand (file[n_files - 1]))
        target_directory = file[--n_files];
      else if (2 < n_files)
        error (EXIT_FAILURE, 0, _("target %s is not a directory"),
               quote (file[n_files - 1]));
    }

  if (backup_suffix_string)
    simple_backup_suffix = xstrdup (backup_suffix_string);

  backup_type = (make_backups
                 ? xget_version (_("backup type"), version_control_string)
                 : no_backups);

  if (target_directory)
    {
      int i;

      /* Create the data structure we'll use to record which hard links we
         create.  Used to ensure that ln detects an obscure corner case that
         might result in user data loss.  Create it only if needed.  */
      if (2 <= n_files
          && remove_existing_files
          /* Don't bother trying to protect symlinks, since ln clobbering
             a just-created symlink won't ever lead to real data loss.  */
          && ! symbolic_link
          /* No destination hard link can be clobbered when making
             numbered backups.  */
          && backup_type != numbered_backups)

        {
          dest_set = hash_initialize (DEST_INFO_INITIAL_CAPACITY,
                                      NULL,
                                      triple_hash,
                                      triple_compare,
                                      triple_free);
          if (dest_set == NULL)
            xalloc_die ();
        }

      ok = true;
      for (i = 0; i < n_files; ++i)
        {
          char *dest_base;
          char *dest = file_name_concat (target_directory,
                                         last_component (file[i]),
                                         &dest_base);
          strip_trailing_slashes (dest_base);
          ok &= do_link (file[i], dest);
          free (dest);
        }
    }
  else
    ok = do_link (file[0], file[1]);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
