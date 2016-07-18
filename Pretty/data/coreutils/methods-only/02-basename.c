int
main (int argc, char **argv)
{
  char *name;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE_NAME, Version,
                      usage, AUTHORS, (char const *) NULL);
  if (getopt_long (argc, argv, "+", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  if (argc < optind + 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  if (optind + 2 < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + 2]));
      usage (EXIT_FAILURE);
    }

  name = base_name (argv[optind]);
  strip_trailing_slashes (name);

  /* Per POSIX, `basename // /' must return `//' on platforms with
     distinct //.  On platforms with drive letters, this generalizes
     to making `basename c: :' return `c:'.  This rule is captured by
     skipping suffix stripping if base_name returned an absolute path
     or a drive letter (only possible if name is a file-system
     root).  */
  if (argc == optind + 2 && IS_RELATIVE_FILE_NAME (name)
      && ! FILE_SYSTEM_PREFIX_LEN (name))
    remove_suffix (name, argv[optind + 1]);

  puts (name);
  free (name);

  exit (EXIT_SUCCESS);
}
