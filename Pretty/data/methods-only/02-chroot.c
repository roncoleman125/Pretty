int
main (int argc, char **argv)
{
  int c;
  char const *userspec = NULL;
  char const *groups = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (EXIT_CANCELED);
  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "+", long_opts, NULL)) != -1)
    {
      switch (c)
        {
        case USERSPEC:
          userspec = optarg;
          break;

        case GROUPS:
          groups = optarg;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_CANCELED);
        }
    }

  if (argc <= optind)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_CANCELED);
    }

  if (chroot (argv[optind]) != 0)
    error (EXIT_CANCELED, errno, _("cannot change root directory to %s"),
           argv[optind]);

  if (chdir ("/"))
    error (EXIT_CANCELED, errno, _("cannot chdir to root directory"));

  if (argc == optind + 1)
    {
      /* No command.  Run an interactive shell.  */
      char *shell = getenv ("SHELL");
      if (shell == NULL)
        shell = bad_cast ("/bin/sh");
      argv[0] = shell;
      argv[1] = bad_cast ("-i");
      argv[2] = NULL;
    }
  else
    {
      /* The following arguments give the command.  */
      argv += optind + 1;
    }

  bool fail = false;

  /* Attempt to set all three: supplementary groups, group ID, user ID.
     Diagnose any failures.  If any have failed, exit before execvp.  */
  if (userspec)
    {
      uid_t uid = -1;
      gid_t gid = -1;
      char *user;
      char *group;
      char const *err = parse_user_spec (userspec, &uid, &gid, &user, &group);

      if (err)
        error (EXIT_CANCELED, errno, "%s", err);

      free (user);
      free (group);

      if (groups && set_additional_groups (groups))
        fail = true;

      if (gid != (gid_t) -1 && setgid (gid))
        {
          error (0, errno, _("failed to set group-ID"));
          fail = true;
        }

      if (uid != (uid_t) -1 && setuid (uid))
        {
          error (0, errno, _("failed to set user-ID"));
          fail = true;
        }
    }
  else
    {
      /* Yes, this call is identical to the one above.
         However, when --userspec and --groups groups are used together,
         we don't want to call this function until after parsing USER:GROUP,
         and it must be called before setuid.  */
      if (groups && set_additional_groups (groups))
        fail = true;
    }

  if (fail)
    exit (EXIT_CANCELED);

  /* Execute the given command.  */
  execvp (argv[0], argv);

  {
    int exit_status = (errno == ENOENT ? EXIT_ENOENT : EXIT_CANNOT_INVOKE);
    error (0, errno, _("failed to run command %s"), quote (argv[0]));
    exit (exit_status);
  }
}
