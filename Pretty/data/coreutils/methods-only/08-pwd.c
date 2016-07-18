int
main (int argc, char **argv)
{
  char *wd;
  /* POSIX requires a default of -L, but most scripts expect -P.  */
  bool logical = (getenv ("POSIXLY_CORRECT") != NULL);

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while (1)
    {
      int c = getopt_long (argc, argv, "LP", longopts, NULL);
      if (c == -1)
        break;
      switch (c)
        {
        case 'L':
          logical = true;
          break;
        case 'P':
          logical = false;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (optind < argc)
    error (0, 0, _("ignoring non-option arguments"));

  if (logical)
    {
      wd = logical_getcwd ();
      if (wd)
        {
          puts (wd);
          exit (EXIT_SUCCESS);
        }
    }

  wd = xgetcwd ();
  if (wd != NULL)
    {
      puts (wd);
      free (wd);
    }
  else
    {
      struct file_name *file_name = file_name_init ();
      robust_getcwd (file_name);
      puts (file_name->start);
      file_name_free (file_name);
    }

  exit (EXIT_SUCCESS);
}
