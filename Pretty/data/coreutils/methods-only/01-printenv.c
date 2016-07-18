int
main (int argc, char **argv)
{
  char **env;
  char *ep, *ap;
  int i;
  bool ok;
  int optc;
  bool opt_nul_terminate_output = false;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (PRINTENV_FAILURE);
  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "+iu:0", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case '0':
          opt_nul_terminate_output = true;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (PRINTENV_FAILURE);
        }
    }

  if (optind >= argc)
    {
      for (env = environ; *env != NULL; ++env)
        printf ("%s%c", *env, opt_nul_terminate_output ? '\0' : '\n');
      ok = true;
    }
  else
    {
      int matches = 0;

      for (i = optind; i < argc; ++i)
        {
          bool matched = false;

          /* 'printenv a=b' is silent, even if 'a=b=c' is in environ.  */
          if (strchr (argv[i], '='))
            continue;

          for (env = environ; *env; ++env)
            {
              ep = *env;
              ap = argv[i];
              while (*ep != '\0' && *ap != '\0' && *ep++ == *ap++)
                {
                  if (*ep == '=' && *ap == '\0')
                    {
                      printf ("%s%c", ep + 1,
                              opt_nul_terminate_output ? '\0' : '\n');
                      matched = true;
                      break;
                    }
                }
            }

          matches += matched;
        }

      ok = (matches == argc - optind);
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
