int
main (int argc, char **argv)
{
  /* Initialize to all zeroes so there is no risk memcmp will report a
     spurious difference in an uninitialized portion of the structure.  */
  DECLARE_ZEROED_AGGREGATE (struct termios, mode);

  enum output_type output_type;
  int optc;
  int argi = 0;
  int opti = 1;
  bool require_set_attr;
  bool speed_was_set;
  bool verbose_output;
  bool recoverable_output;
  int k;
  bool noargs = true;
  char *file_name = NULL;
  const char *device_name;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  output_type = changed;
  verbose_output = false;
  recoverable_output = false;

  /* Don't print error messages for unrecognized options.  */
  opterr = 0;

  /* If any new options are ever added to stty, the short options MUST
     NOT allow any ambiguity with the stty settings.  For example, the
     stty setting "-gagFork" would not be feasible, since it will be
     parsed as "-g -a -g -F ork".  If you change anything about how
     stty parses options, be sure it still works with combinations of
     short and long options, --, POSIXLY_CORRECT, etc.  */

  while ((optc = getopt_long (argc - argi, argv + argi, "-agF:",
                              longopts, NULL))
         != -1)
    {
      switch (optc)
        {
        case 'a':
          verbose_output = true;
          output_type = all;
          break;

        case 'g':
          recoverable_output = true;
          output_type = recoverable;
          break;

        case 'F':
          if (file_name)
            error (EXIT_FAILURE, 0, _("only one device may be specified"));
          file_name = optarg;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          noargs = false;

          /* Skip the argument containing this unrecognized option;
             the 2nd pass will analyze it.  */
          argi += opti;

          /* Restart getopt_long from the first unskipped argument.  */
          opti = 1;
          optind = 0;

          break;
        }

      /* Clear fully-parsed arguments, so they don't confuse the 2nd pass.  */
      while (opti < optind)
        argv[argi + opti++] = NULL;
    }

  /* Specifying both -a and -g gets an error.  */
  if (verbose_output && recoverable_output)
    error (EXIT_FAILURE, 0,
           _("the options for verbose and stty-readable output styles are\n"
             "mutually exclusive"));

  /* Specifying any other arguments with -a or -g gets an error.  */
  if (!noargs && (verbose_output || recoverable_output))
    error (EXIT_FAILURE, 0,
           _("when specifying an output style, modes may not be set"));

  /* FIXME: it'd be better not to open the file until we've verified
     that all arguments are valid.  Otherwise, we could end up doing
     only some of the requested operations and then failing, probably
     leaving things in an undesirable state.  */

  if (file_name)
    {
      int fdflags;
      device_name = file_name;
      if (fd_reopen (STDIN_FILENO, device_name, O_RDONLY | O_NONBLOCK, 0) < 0)
        error (EXIT_FAILURE, errno, "%s", device_name);
      if ((fdflags = fcntl (STDIN_FILENO, F_GETFL)) == -1
          || fcntl (STDIN_FILENO, F_SETFL, fdflags & ~O_NONBLOCK) < 0)
        error (EXIT_FAILURE, errno, _("%s: couldn't reset non-blocking mode"),
               device_name);
    }
  else
    device_name = _("standard input");

  if (tcgetattr (STDIN_FILENO, &mode))
    error (EXIT_FAILURE, errno, "%s", device_name);

  if (verbose_output || recoverable_output || noargs)
    {
      max_col = screen_columns ();
      current_col = 0;
      display_settings (output_type, &mode, device_name);
      exit (EXIT_SUCCESS);
    }

  speed_was_set = false;
  require_set_attr = false;
  for (k = 1; k < argc; k++)
    {
      char const *arg = argv[k];
      bool match_found = false;
      bool reversed = false;
      int i;

      if (! arg)
        continue;

      if (arg[0] == '-')
        {
          ++arg;
          reversed = true;
        }
      for (i = 0; mode_info[i].name != NULL; ++i)
        {
          if (STREQ (arg, mode_info[i].name))
            {
              match_found = set_mode (&mode_info[i], reversed, &mode);
              require_set_attr = true;
              break;
            }
        }
      if (!match_found && reversed)
        {
          error (0, 0, _("invalid argument %s"), quote (arg - 1));
          usage (EXIT_FAILURE);
        }
      if (!match_found)
        {
          for (i = 0; control_info[i].name != NULL; ++i)
            {
              if (STREQ (arg, control_info[i].name))
                {
                  if (k == argc - 1)
                    {
                      error (0, 0, _("missing argument to %s"), quote (arg));
                      usage (EXIT_FAILURE);
                    }
                  match_found = true;
                  ++k;
                  set_control_char (&control_info[i], argv[k], &mode);
                  require_set_attr = true;
                  break;
                }
            }
        }
      if (!match_found)
        {
          if (STREQ (arg, "ispeed"))
            {
              if (k == argc - 1)
                {
                  error (0, 0, _("missing argument to %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              ++k;
              set_speed (input_speed, argv[k], &mode);
              speed_was_set = true;
              require_set_attr = true;
            }
          else if (STREQ (arg, "ospeed"))
            {
              if (k == argc - 1)
                {
                  error (0, 0, _("missing argument to %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              ++k;
              set_speed (output_speed, argv[k], &mode);
              speed_was_set = true;
              require_set_attr = true;
            }
          else if (STREQ (arg, "rows"))
            {
              if (k == argc - 1)
                {
                  error (0, 0, _("missing argument to %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              ++k;
              set_window_size (integer_arg (argv[k], INT_MAX), -1,
                               device_name);
            }
          else if (STREQ (arg, "cols")
                   || STREQ (arg, "columns"))
            {
              if (k == argc - 1)
                {
                  error (0, 0, _("missing argument to %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              ++k;
              set_window_size (-1, integer_arg (argv[k], INT_MAX),
                               device_name);
            }
          else if (STREQ (arg, "size"))
            {
              max_col = screen_columns ();
              current_col = 0;
              display_window_size (false, device_name);
            }
          else if (STREQ (arg, "line"))
            {
              unsigned long int value;
              if (k == argc - 1)
                {
                  error (0, 0, _("missing argument to %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              ++k;
              mode.c_line = value = integer_arg (argv[k], ULONG_MAX);
              if (mode.c_line != value)
                error (0, 0, _("invalid line discipline %s"), quote (argv[k]));
              require_set_attr = true;
            }
          else if (STREQ (arg, "speed"))
            {
              max_col = screen_columns ();
              display_speed (&mode, false);
            }
          else if (string_to_baud (arg) != (speed_t) -1)
            {
              set_speed (both_speeds, arg, &mode);
              speed_was_set = true;
              require_set_attr = true;
            }
          else
            {
              if (! recover_mode (arg, &mode))
                {
                  error (0, 0, _("invalid argument %s"), quote (arg));
                  usage (EXIT_FAILURE);
                }
              require_set_attr = true;
            }
        }
    }

  if (require_set_attr)
    {
      /* Initialize to all zeroes so there is no risk memcmp will report a
         spurious difference in an uninitialized portion of the structure.  */
      DECLARE_ZEROED_AGGREGATE (struct termios, new_mode);

      if (tcsetattr (STDIN_FILENO, TCSADRAIN, &mode))
        error (EXIT_FAILURE, errno, "%s", device_name);

      /* POSIX (according to Zlotnick's book) tcsetattr returns zero if
         it performs *any* of the requested operations.  This means it
         can report `success' when it has actually failed to perform
         some proper subset of the requested operations.  To detect
         this partial failure, get the current terminal attributes and
         compare them to the requested ones.  */

      if (tcgetattr (STDIN_FILENO, &new_mode))
        error (EXIT_FAILURE, errno, "%s", device_name);

      /* Normally, one shouldn't use memcmp to compare structures that
         may have `holes' containing uninitialized data, but we have been
         careful to initialize the storage of these two variables to all
         zeroes.  One might think it more efficient simply to compare the
         modified fields, but that would require enumerating those fields --
         and not all systems have the same fields in this structure.  */

      if (memcmp (&mode, &new_mode, sizeof (mode)) != 0)
        {
          /* SunOS 4.1.3 (at least) has the problem that after this sequence,
             tcgetattr (&m1); tcsetattr (&m1); tcgetattr (&m2);
             sometimes (m1 != m2).  The only difference is in the four bits
             of the c_cflag field corresponding to the baud rate.  To save
             Sun users a little confusion, don't report an error if this
             happens.  But suppress the error only if we haven't tried to
             set the baud rate explicitly -- otherwise we'd never give an
             error for a true failure to set the baud rate.  */

          new_mode.c_cflag &= (~CIBAUD);
          if (speed_was_set || memcmp (&mode, &new_mode, sizeof (mode)) != 0)
            {
              error (EXIT_FAILURE, 0,
                     _("%s: unable to perform all requested operations"),
                     device_name);
              {
                size_t i;
                printf ("new_mode: mode\n");
                for (i = 0; i < sizeof (new_mode); i++)
                  printf ("0x%02x: 0x%02x\n",
                          *(((unsigned char *) &new_mode) + i),
                          *(((unsigned char *) &mode) + i));
              }
            }
        }
    }

  exit (EXIT_SUCCESS);
}

