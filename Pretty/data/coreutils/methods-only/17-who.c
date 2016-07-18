int
main (int argc, char **argv)
{
  int optc;
  bool assumptions = true;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "abdlmpqrstuwHT", longopts, NULL))
         != -1)
    {
      switch (optc)
        {
        case 'a':
          need_boottime = true;
          need_deadprocs = true;
          need_login = true;
          need_initspawn = true;
          need_runlevel = true;
          need_clockchange = true;
          need_users = true;
          include_mesg = true;
          include_idle = true;
          include_exit = true;
          assumptions = false;
          break;

        case 'b':
          need_boottime = true;
          assumptions = false;
          break;

        case 'd':
          need_deadprocs = true;
          include_idle = true;
          include_exit = true;
          assumptions = false;
          break;

        case 'H':
          include_heading = true;
          break;

        case 'l':
          need_login = true;
          include_idle = true;
          assumptions = false;
          break;

        case 'm':
          my_line_only = true;
          break;

        case 'p':
          need_initspawn = true;
          assumptions = false;
          break;

        case 'q':
          short_list = true;
          break;

        case 'r':
          need_runlevel = true;
          include_idle = true;
          assumptions = false;
          break;

        case 's':
          short_output = true;
          break;

        case 't':
          need_clockchange = true;
          assumptions = false;
          break;

        case 'T':
        case 'w':
          include_mesg = true;
          break;

        case 'u':
          need_users = true;
          include_idle = true;
          assumptions = false;
          break;

        case LOOKUP_OPTION:
          do_lookup = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (assumptions)
    {
      need_users = true;
      short_output = true;
    }

  if (include_exit)
    {
      short_output = false;
    }

  if (hard_locale (LC_TIME))
    {
      time_format = "%Y-%m-%d %H:%M";
      time_format_width = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2;
    }
  else
    {
      time_format = "%b %e %H:%M";
      time_format_width = 3 + 1 + 2 + 1 + 2 + 1 + 2;
    }

  switch (argc - optind)
    {
    case 2:			/* who <blurf> <glop> */
      my_line_only = true;
      /* Fall through.  */
    case -1:
    case 0:			/* who */
      who (UTMP_FILE, READ_UTMP_CHECK_PIDS);
      break;

    case 1:			/* who <utmp file> */
      who (argv[optind], 0);
      break;

    default:			/* lose */
      error (0, 0, _("extra operand %s"), quote (argv[optind + 2]));
      usage (EXIT_FAILURE);
    }

  exit (EXIT_SUCCESS);
}
