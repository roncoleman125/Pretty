int
main (int argc, char **argv)
{
  /* Recognize --help or --version only if it's the only command-line
     argument.  */
  if (argc == 2)
    {
      initialize_main (&argc, &argv);
      set_program_name (argv[0]);
      setlocale (LC_ALL, "");
      bindtextdomain (PACKAGE, LOCALEDIR);
      textdomain (PACKAGE);

      atexit (close_stdout);

      if (STREQ (argv[1], "--help"))
        usage (EXIT_STATUS);

      if (STREQ (argv[1], "--version"))
        version_etc (stdout, PROGRAM_NAME, PACKAGE_NAME, Version, AUTHORS,
                     (char *) NULL);
    }

  exit (EXIT_STATUS);
}
