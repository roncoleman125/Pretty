int
main (int argc, char **argv)
{
  int c;
  bool ok;
  bool make_backups = false;
  char *backup_suffix_string;
  char *version_control_string = NULL;
  struct cp_options x;
  bool copy_contents = false;
  char *target_directory = NULL;
  bool no_target_directory = false;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdin);

  selinux_enabled = (0 < is_selinux_enabled ());
  cp_option_init (&x);

  /* FIXME: consider not calling getenv for SIMPLE_BACKUP_SUFFIX unless
     we'll actually use backup_suffix_string.  */
  backup_suffix_string = getenv ("SIMPLE_BACKUP_SUFFIX");

  while ((c = getopt_long (argc, argv, "abdfHilLnprst:uvxPRS:T",
                           long_opts, NULL))
         != -1)
    {
      switch (c)
        {
        case SPARSE_OPTION:
          x.sparse_mode = XARGMATCH ("--sparse", optarg,
                                     sparse_type_string, sparse_type);
          break;

        case REFLINK_OPTION:
          if (optarg == NULL)
            x.reflink_mode = REFLINK_ALWAYS;
          else
            x.reflink_mode = XARGMATCH ("--reflink", optarg,
                                       reflink_type_string, reflink_type);
          break;

        case 'a':
          /* Like -dR --preserve=all with reduced failure diagnostics.  */
          x.dereference = DEREF_NEVER;
          x.preserve_links = true;
          x.preserve_ownership = true;
          x.preserve_mode = true;
          x.preserve_timestamps = true;
          x.require_preserve = true;
          if (selinux_enabled)
             x.preserve_security_context = true;
          x.preserve_xattr = true;
          x.reduce_diagnostics = true;
          x.recursive = true;
          break;

        case 'b':
          make_backups = true;
          if (optarg)
            version_control_string = optarg;
          break;

        case ATTRIBUTES_ONLY_OPTION:
          x.data_copy_required = false;
          break;

        case COPY_CONTENTS_OPTION:
          copy_contents = true;
          break;

        case 'd':
          x.preserve_links = true;
          x.dereference = DEREF_NEVER;
          break;

        case 'f':
          x.unlink_dest_after_failed_open = true;
          break;

        case 'H':
          x.dereference = DEREF_COMMAND_LINE_ARGUMENTS;
          break;

        case 'i':
          x.interactive = I_ASK_USER;
          break;

        case 'l':
          x.hard_link = true;
          break;

        case 'L':
          x.dereference = DEREF_ALWAYS;
          break;

        case 'n':
          x.interactive = I_ALWAYS_NO;
          break;

        case 'P':
          x.dereference = DEREF_NEVER;
          break;

        case NO_PRESERVE_ATTRIBUTES_OPTION:
          decode_preserve_arg (optarg, &x, false);
          break;

        case PRESERVE_ATTRIBUTES_OPTION:
          if (optarg == NULL)
            {
              /* Fall through to the case for `p' below.  */
            }
          else
            {
              decode_preserve_arg (optarg, &x, true);
              x.require_preserve = true;
              break;
            }

        case 'p':
          x.preserve_ownership = true;
          x.preserve_mode = true;
          x.preserve_timestamps = true;
          x.require_preserve = true;
          break;

        case PARENTS_OPTION:
          parents_option = true;
          break;

        case 'r':
        case 'R':
          x.recursive = true;
          break;

        case UNLINK_DEST_BEFORE_OPENING:
          x.unlink_dest_before_opening = true;
          break;

        case STRIP_TRAILING_SLASHES_OPTION:
          remove_trailing_slashes = true;
          break;

        case 's':
          x.symbolic_link = true;
          break;

        case 't':
          if (target_directory)
            error (EXIT_FAILURE, 0,
                   _("multiple target directories specified"));
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

        case 'x':
          x.one_file_system = true;
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

  if (x.hard_link && x.symbolic_link)
    {
      error (0, 0, _("cannot make both hard and symbolic links"));
      usage (EXIT_FAILURE);
    }

  if (make_backups && x.interactive == I_ALWAYS_NO)
    {
      error (0, 0,
             _("options --backup and --no-clobber are mutually exclusive"));
      usage (EXIT_FAILURE);
    }

  if (x.reflink_mode == REFLINK_ALWAYS && x.sparse_mode != SPARSE_AUTO)
    {
      error (0, 0, _("--reflink can be used only with --sparse=auto"));
      usage (EXIT_FAILURE);
    }

  if (backup_suffix_string)
    simple_backup_suffix = xstrdup (backup_suffix_string);

  x.backup_type = (make_backups
                   ? xget_version (_("backup type"),
                                   version_control_string)
                   : no_backups);

  if (x.dereference == DEREF_UNDEFINED)
    {
      if (x.recursive)
        /* This is compatible with FreeBSD.  */
        x.dereference = DEREF_NEVER;
      else
        x.dereference = DEREF_ALWAYS;
    }

  if (x.recursive)
    x.copy_as_regular = copy_contents;

  /* If --force (-f) was specified and we're in link-creation mode,
     first remove any existing destination file.  */
  if (x.unlink_dest_after_failed_open && (x.hard_link || x.symbolic_link))
    x.unlink_dest_before_opening = true;

  if (x.preserve_security_context)
    {
      if (!selinux_enabled)
        error (EXIT_FAILURE, 0,
               _("cannot preserve security context "
                 "without an SELinux-enabled kernel"));
    }

  if (x.require_preserve_xattr)
    error (EXIT_FAILURE, 0, _("cannot preserve extended attributes, cp is "
                              "built without xattr support"));

  /* Allocate space for remembering copied and created files.  */

  hash_init ();

  ok = do_copy (argc - optind, argv + optind,
                target_directory, no_target_directory, &x);

  forget_all ();

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
