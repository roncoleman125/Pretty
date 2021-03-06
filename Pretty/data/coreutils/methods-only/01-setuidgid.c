int
main (int argc, char **argv)
{
  uid_t uid;
  GETGROUPS_T *gids = NULL;
  size_t n_gids = 0;
  size_t n_gids_allocated = 0;
  gid_t primary_gid;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (SETUIDGID_FAILURE);
  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE_NAME, Version,
                      usage, AUTHORS, (char const *) NULL);
  {
    int c;
    while ((c = getopt_long (argc, argv, "+g:", NULL, NULL)) != -1)
      {
        switch (c)
          {
            case 'g':
              {
                unsigned long int tmp_ul;
                char *gr = optarg;
                char *ptr;
                while (true)
                  {
                    if (! (xstrtoul (gr, &ptr, 10, &tmp_ul, NULL) == LONGINT_OK
                           && tmp_ul <= GID_T_MAX))
                      error (EXIT_FAILURE, 0, _("invalid group %s"),
                             quote (gr));
                    if (n_gids == n_gids_allocated)
                      gids = X2NREALLOC (gids, &n_gids_allocated);
                    gids[n_gids++] = tmp_ul;

                    if (*ptr == '\0')
                      break;
                    if (*ptr != ',')
                      {
                        error (0, 0, _("invalid group %s"), quote (gr));
                        usage (SETUIDGID_FAILURE);
                      }
                    gr = ptr + 1;
                  }
                break;
              }

            default:
              usage (SETUIDGID_FAILURE);
          }
      }
  }

  if (argc <= optind + 1)
    {
      if (argc < optind + 1)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[optind]));
      usage (SETUIDGID_FAILURE);
    }

  {
    const struct passwd *pwd;
    unsigned long int tmp_ul;
    char *user = argv[optind];
    char *ptr;
    bool have_uid = false;

    if (xstrtoul (user, &ptr, 10, &tmp_ul, "") == LONGINT_OK
        && tmp_ul <= UID_T_MAX)
      {
        uid = tmp_ul;
        have_uid = true;
      }

    if (!have_uid)
      {
        pwd = getpwnam (user);
        if (pwd == NULL)
          {
            error (SETUIDGID_FAILURE, errno,
                   _("unknown user-ID: %s"), quote (user));
            usage (SETUIDGID_FAILURE);
          }
        uid = pwd->pw_uid;
      }
    else if (n_gids == 0)
      {
        pwd = getpwuid (uid);
        if (pwd == NULL)
          {
            error (SETUIDGID_FAILURE, errno,
                   _("to use user-ID %s you need to use -g too"), quote (user));
            usage (SETUIDGID_FAILURE);
          }
      }

    if (n_gids == 0)
      {
        int n = xgetgroups (pwd->pw_name, pwd->pw_gid, &gids);
        if (n <= 0)
          error (EXIT_FAILURE, errno, _("failed to get groups for user %s"),
                 quote (pwd->pw_name));
        n_gids = n;
      }

    if (setgroups (n_gids, gids))
      error (SETUIDGID_FAILURE, errno,
             _("failed to set supplemental group(s)"));

    primary_gid = gids[0];
    primary_gid = pwd->pw_gid;
  }

  if (setgid (primary_gid))
    error (SETUIDGID_FAILURE, errno,
           _("cannot set group-ID to %lu"), (unsigned long int) primary_gid);

  if (setuid (uid))
    error (SETUIDGID_FAILURE, errno,
           _("cannot set user-ID to %lu"), (unsigned long int) uid);

  {
    char **cmd = argv + optind + 1;
    int exit_status;
    execvp (*cmd, cmd);
    exit_status = (errno == ENOENT ? EXIT_ENOENT : EXIT_CANNOT_INVOKE);

    error (0, errno, _("failed to run command %s"), quote (*cmd));
    exit (exit_status);
  }
}
