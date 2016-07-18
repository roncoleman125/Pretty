int
main (int argc, char **argv)
{
  int optc;
  int selinux_enabled = (is_selinux_enabled () > 0);

  /* If true, output the list of all group IDs. -G */
  bool just_group_list = false;
  /* If true, output only the group ID(s). -g */
  bool just_group = false;
  /* If true, output real UID/GID instead of default effective UID/GID. -r */
  bool use_real = false;
  /* If true, output only the user ID(s). -u */
  bool just_user = false;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "agnruGZ", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'a':
          /* Ignore -a, for compatibility with SVR4.  */
          break;

        case 'Z':
          /* politely decline if we're not on a selinux-enabled kernel. */
          if (!selinux_enabled)
            error (EXIT_FAILURE, 0,
                   _("--context (-Z) works only on an SELinux-enabled kernel"));
          just_context = 1;
          break;

        case 'g':
          just_group = true;
          break;
        case 'n':
          use_name = true;
          break;
        case 'r':
          use_real = true;
          break;
        case 'u':
          just_user = true;
          break;
        case 'G':
          just_group_list = true;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (1 < argc - optind)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
      usage (EXIT_FAILURE);
    }

  if (argc - optind == 1 && just_context)
    error (EXIT_FAILURE, 0,
           _("cannot print security context when user specified"));

  /* If we are on a selinux-enabled kernel and no user is specified,
     get our context. Otherwise, leave the context variable alone -
     it has been initialized known invalid value and will be not
     displayed in print_full_info() */
  if (selinux_enabled && argc == optind)
    {
      if (getcon (&context) && just_context)
        error (EXIT_FAILURE, 0, _("can't get process context"));
    }

  if (just_user + just_group + just_group_list + just_context > 1)
    error (EXIT_FAILURE, 0, _("cannot print \"only\" of more than one choice"));

  if (just_user + just_group + just_group_list == 0 && (use_real || use_name))
    error (EXIT_FAILURE, 0,
           _("cannot print only names or real IDs in default format"));

  if (argc - optind == 1)
    {
      struct passwd *pwd = getpwnam (argv[optind]);
      if (pwd == NULL)
        error (EXIT_FAILURE, 0, _("%s: No such user"), argv[optind]);
      ruid = euid = pwd->pw_uid;
      rgid = egid = pwd->pw_gid;
    }
  else
    {
      euid = geteuid ();
      ruid = getuid ();
      egid = getegid ();
      rgid = getgid ();
    }

  if (just_user)
    {
      print_user (use_real ? ruid : euid);
    }
  else if (just_group)
    {
      if (!print_group (use_real ? rgid : egid, use_name))
        ok = false;
    }
  else if (just_group_list)
    {
      if (!print_group_list (argv[optind], ruid, rgid, egid, use_name))
        ok = false;
    }
  else if (just_context)
    {
      fputs (context, stdout);
    }
  else
    {
      print_full_info (argv[optind]);
    }
  putchar ('\n');

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}

