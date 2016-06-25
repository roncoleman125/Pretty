int
main (int argc, char **argv)
{
  bool preserve_root = false;

  uid_t uid = -1;	/* Specified uid; -1 if not to be changed. */
  gid_t gid = -1;	/* Specified gid; -1 if not to be changed. */

  /* Change the owner (group) of a file only if it has this uid (gid).
     -1 means there's no restriction.  */
  uid_t required_uid = -1;
  gid_t required_gid = -1;

  /* Bit flags that control how fts works.  */
  int bit_flags = FTS_PHYSICAL;

  /* 1 if --dereference, 0 if --no-dereference, -1 if neither has been
     specified.  */
  int dereference = -1;

  struct Chown_option chopt;
  bool ok;
  int optc;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  chopt_init (&chopt);

  while ((optc = getopt_long (argc, argv, "HLPRcfhv", long_options, NULL))
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

        case FROM_OPTION:
          {
            char *u_dummy, *g_dummy;
            const char *e = parse_user_spec (optarg,
                                             &required_uid, &required_gid,
                                             &u_dummy, &g_dummy);
            if (e)
              error (EXIT_FAILURE, 0, "%s: %s", e, quote (optarg));
            break;
          }

        case 'R':
          chopt.recurse = true;
          break;

        case 'c':
          chopt.verbosity = V_changes_only;
          break;

        case 'f':
          chopt.force_silent = true;
          break;

        case 'v':
          chopt.verbosity = V_high;
          break;

        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (chopt.recurse)
    {
      if (bit_flags == FTS_PHYSICAL)
        {
          if (dereference == 1)
            error (EXIT_FAILURE, 0,
                   _("-R --dereference requires either -H or -L"));
          dereference = 0;
        }
    }
  else
    {
      bit_flags = FTS_PHYSICAL;
    }
  chopt.affect_symlink_referent = (dereference != 0);

  if (argc - optind < (reference_file ? 1 : 2))
    {
      if (argc <= optind)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      usage (EXIT_FAILURE);
    }

  if (reference_file)
    {
      struct stat ref_stats;
      if (stat (reference_file, &ref_stats))
        error (EXIT_FAILURE, errno, _("failed to get attributes of %s"),
               quote (reference_file));

      uid = ref_stats.st_uid;
      gid = ref_stats.st_gid;
      chopt.user_name = uid_to_name (ref_stats.st_uid);
      chopt.group_name = gid_to_name (ref_stats.st_gid);
    }
  else
    {
      const char *e = parse_user_spec (argv[optind], &uid, &gid,
                                       &chopt.user_name, &chopt.group_name);
      if (e)
        error (EXIT_FAILURE, 0, "%s: %s", e, quote (argv[optind]));

      /* If a group is specified but no user, set the user name to the
         empty string so that diagnostics say "ownership :GROUP"
         rather than "group GROUP".  */
      if (!chopt.user_name && chopt.group_name)
        chopt.user_name = bad_cast ("");

      optind++;
    }

  if (chopt.recurse && preserve_root)
    {
      static struct dev_ino dev_ino_buf;
      chopt.root_dev_ino = get_root_dev_ino (&dev_ino_buf);
      if (chopt.root_dev_ino == NULL)
        error (EXIT_FAILURE, errno, _("failed to get attributes of %s"),
               quote ("/"));
    }

  bit_flags |= FTS_DEFER_STAT;
  ok = chown_files (argv + optind, bit_flags,
                    uid, gid,
                    required_uid, required_gid, &chopt);

  chopt_free (&chopt);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
