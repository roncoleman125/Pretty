int
main (int argc, char **argv)
{
  int i;
  double seconds = 0.0;
  bool ok = true;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE_NAME, Version,
                      usage, AUTHORS, (char const *) NULL);
  if (getopt_long (argc, argv, "", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  if (argc == 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXIT_FAILURE);
    }

  for (i = optind; i < argc; i++)
    {
      double s;
      const char *p;
      if (! xstrtod (argv[i], &p, &s, c_strtod)
          /* Nonnegative interval.  */
          || ! (0 <= s)
          /* No extra chars after the number and an optional s,m,h,d char.  */
          || (*p && *(p+1))
          /* Check any suffix char and update S based on the suffix.  */
          || ! apply_suffix (&s, *p))
        {
          error (0, 0, _("invalid time interval %s"), quote (argv[i]));
          ok = false;
        }

      seconds += s;
    }

  if (!ok)
    usage (EXIT_FAILURE);

  if (xnanosleep (seconds))
    error (EXIT_FAILURE, errno, _("cannot read realtime clock"));

  exit (EXIT_SUCCESS);
}
