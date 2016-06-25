int
main (int argc, char **argv)
{
  int optc;
  bool ok = true;
  gid_t rgid, egid;
  uid_t ruid;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  /* Processing the arguments this way makes groups.c behave differently to
   * groups.sh if one of the arguments is "--".
   */
  while ((optc = getopt_long (argc, argv, "", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (optind == argc)
    {
      /* No arguments.  Divulge the details of the current process. */
      ruid = getuid ();
      egid = getegid ();
      rgid = getgid ();

      if (!print_group_list (NULL, ruid, rgid, egid, true))
        ok = false;
      putchar ('\n');
    }
  else
    {
      /* At least one argument.  Divulge the details of the specified users. */
      while (optind < argc)
        {
          struct passwd *pwd = getpwnam (argv[optind]);
          if (pwd == NULL)
            error (EXIT_FAILURE, 0, _("%s: No such user"), argv[optind]);
          ruid = pwd->pw_uid;
          rgid = egid = pwd->pw_gid;

          printf ("%s : ", argv[optind]);
          if (!print_group_list (argv[optind++], ruid, rgid, egid, true))
            ok = false;
          putchar ('\n');
        }
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
