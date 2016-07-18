int
main (int argc, char **argv)
{
  int optc;
  const char *new_user = DEFAULT_USER;
  char *command = NULL;
  char *shell = NULL;
  struct passwd *pw;
  struct passwd pw_copy;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (EXIT_CANCELED);
  atexit (close_stdout);

  fast_startup = false;
  simulate_login = false;
  change_environment = true;

  while ((optc = getopt_long (argc, argv, "c:flmps:", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'c':
          command = optarg;
          break;

        case 'f':
          fast_startup = true;
          break;

        case 'l':
          simulate_login = true;
          break;

        case 'm':
        case 'p':
          change_environment = false;
          break;

        case 's':
          shell = optarg;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_CANCELED);
        }
    }

  if (optind < argc && STREQ (argv[optind], "-"))
    {
      simulate_login = true;
      ++optind;
    }
  if (optind < argc)
    new_user = argv[optind++];

  pw = getpwnam (new_user);
  if (! (pw && pw->pw_name && pw->pw_name[0] && pw->pw_dir && pw->pw_dir[0]
         && pw->pw_passwd))
    error (EXIT_CANCELED, 0, _("user %s does not exist"), new_user);

  /* Make a copy of the password information and point pw at the local
     copy instead.  Otherwise, some systems (e.g. GNU/Linux) would clobber
     the static data through the getlogin call from log_su.
     Also, make sure pw->pw_shell is a nonempty string.
     It may be NULL when NEW_USER is a username that is retrieved via NIS (YP),
     but that doesn't have a default shell listed.  */
  pw_copy = *pw;
  pw = &pw_copy;
  pw->pw_name = xstrdup (pw->pw_name);
  pw->pw_passwd = xstrdup (pw->pw_passwd);
  pw->pw_dir = xstrdup (pw->pw_dir);
  pw->pw_shell = xstrdup (pw->pw_shell && pw->pw_shell[0]
                          ? pw->pw_shell
                          : DEFAULT_SHELL);
  endpwent ();

  if (!correct_password (pw))
    {
      log_su (pw, false);
      error (EXIT_CANCELED, 0, _("incorrect password"));
    }
  else
    {
      log_su (pw, true);
    }

  if (!shell && !change_environment)
    shell = getenv ("SHELL");
  if (shell && getuid () != 0 && restricted_shell (pw->pw_shell))
    {
      /* The user being su'd to has a nonstandard shell, and so is
         probably a uucp account or has restricted access.  Don't
         compromise the account by allowing access with a standard
         shell.  */
      error (0, 0, _("using restricted shell %s"), pw->pw_shell);
      shell = NULL;
    }
  shell = xstrdup (shell ? shell : pw->pw_shell);
  modify_environment (pw, shell);

  change_identity (pw);
  if (simulate_login && chdir (pw->pw_dir) != 0)
    error (0, errno, _("warning: cannot change directory to %s"), pw->pw_dir);

  /* error() flushes stderr, but does not check for write failure.
     Normally, we would catch this via our atexit() hook of
     close_stdout, but execv() gets in the way.  If stderr
     encountered a write failure, there is no need to try calling
     error() again.  */
  if (ferror (stderr))
    exit (EXIT_CANCELED);

  run_shell (shell, command, argv + optind, MAX (0, argc - optind));
}
