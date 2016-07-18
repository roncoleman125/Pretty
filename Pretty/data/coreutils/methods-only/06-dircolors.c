int
main (int argc, char **argv)
{
  bool ok = true;
  int optc;
  enum Shell_syntax syntax = SHELL_SYNTAX_UNKNOWN;
  bool print_database = false;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "bcp", long_options, NULL)) != -1)
    switch (optc)
      {
      case 'b':	/* Bourne shell syntax.  */
        syntax = SHELL_SYNTAX_BOURNE;
        break;

      case 'c':	/* C shell syntax.  */
        syntax = SHELL_SYNTAX_C;
        break;

      case 'p':
        print_database = true;
        break;

      case_GETOPT_HELP_CHAR;

      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

      default:
        usage (EXIT_FAILURE);
      }

  argc -= optind;
  argv += optind;

  /* It doesn't make sense to use --print with either of
     --bourne or --c-shell.  */
  if (print_database && syntax != SHELL_SYNTAX_UNKNOWN)
    {
      error (0, 0,
             _("the options to output dircolors' internal database and\n\
to select a shell syntax are mutually exclusive"));
      usage (EXIT_FAILURE);
    }

  if (!print_database < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[!print_database]));
      if (print_database)
        fprintf (stderr, "%s\n",
                 _("file operands cannot be combined with "
                   "--print-database (-p)"));
      usage (EXIT_FAILURE);
    }

  if (print_database)
    {
      char const *p = G_line;
      while (p < G_line + sizeof G_line)
        {
          puts (p);
          p += strlen (p) + 1;
        }
    }
  else
    {
      /* If shell syntax was not explicitly specified, try to guess it. */
      if (syntax == SHELL_SYNTAX_UNKNOWN)
        {
          syntax = guess_shell_syntax ();
          if (syntax == SHELL_SYNTAX_UNKNOWN)
            {
              error (EXIT_FAILURE, 0,
         _("no SHELL environment variable, and no shell type option given"));
            }
        }

      obstack_init (&lsc_obstack);
      if (argc == 0)
        ok = dc_parse_stream (NULL, NULL);
      else
        ok = dc_parse_file (argv[0]);

      if (ok)
        {
          size_t len = obstack_object_size (&lsc_obstack);
          char *s = obstack_finish (&lsc_obstack);
          const char *prefix;
          const char *suffix;

          if (syntax == SHELL_SYNTAX_BOURNE)
            {
              prefix = "LS_COLORS='";
              suffix = "';\nexport LS_COLORS\n";
            }
          else
            {
              prefix = "setenv LS_COLORS '";
              suffix = "'\n";
            }
          fputs (prefix, stdout);
          fwrite (s, 1, len, stdout);
          fputs (suffix, stdout);
        }
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
