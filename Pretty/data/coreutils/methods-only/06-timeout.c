int
main (int argc, char **argv)
{
  unsigned long timeout;
  char signame[SIG2STR_MAX];
  int c;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (EXIT_CANCELED);
  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "+k:s:", long_options, NULL)) != -1)
    {
      switch (c)
        {
        case 'k':
          kill_after = parse_duration (optarg);
          break;

        case 's':
          term_signal = operand2sig (optarg, signame);
          if (term_signal == -1)
            usage (EXIT_CANCELED);
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_CANCELED);
          break;
        }
    }

  if (argc - optind < 2)
    usage (EXIT_CANCELED);

  timeout = parse_duration (argv[optind++]);

  argv += optind;

  /* Ensure we're in our own group so all subprocesses can be killed.
     Note we don't just put the child in a separate group as
     then we would need to worry about foreground and background groups
     and propagating signals between them.  */
  setpgid (0, 0);

  /* Setup handlers before fork() so that we
     handle any signals caused by child, without races.  */
  install_signal_handlers (term_signal);
  signal (SIGTTIN, SIG_IGN);    /* don't sTop if background child needs tty.  */
  signal (SIGTTOU, SIG_IGN);    /* don't sTop if background child needs tty.  */
  signal (SIGCHLD, SIG_DFL);    /* Don't inherit CHLD handling from parent.   */

  monitored_pid = fork ();
  if (monitored_pid == -1)
    {
      error (0, errno, _("fork system call failed"));
      return EXIT_CANCELED;
    }
  else if (monitored_pid == 0)
    {                           /* child */
      int exit_status;

      /* exec doesn't reset SIG_IGN -> SIG_DFL.  */
      signal (SIGTTIN, SIG_DFL);
      signal (SIGTTOU, SIG_DFL);

      execvp (argv[0], argv);   /* FIXME: should we use "sh -c" ... here?  */

      /* exit like sh, env, nohup, ...  */
      exit_status = (errno == ENOENT ? EXIT_ENOENT : EXIT_CANNOT_INVOKE);
      error (0, errno, _("failed to run command %s"), quote (argv[0]));
      return exit_status;
    }
  else
    {
      int status;

      alarm (timeout);

      /* We're just waiting for a single process here, so wait() suffices.
         Note the signal() calls above on GNU/Linux and BSD at least,
         essentially call the lower level sigaction() with the SA_RESTART flag
         set, which ensures the following wait call will only return if the
         child exits, not on this process receiving a signal. Also we're not
         passing WUNTRACED | WCONTINUED to a waitpid() call and so will not get
         indication that the child has stopped or continued.  */
      if (wait (&status) == -1)
        {
          /* shouldn't happen.  */
          error (0, errno, _("error waiting for command"));
          status = EXIT_CANCELED;
        }
      else
        {
          if (WIFEXITED (status))
            status = WEXITSTATUS (status);
          else if (WIFSIGNALED (status))
            status = WTERMSIG (status) + 128; /* what sh does at least.  */
          else
            {
              /* shouldn't happen.  */
              error (0, 0, _("unknown status from command (0x%X)"), status);
              status = EXIT_FAILURE;
            }
        }

      if (timed_out)
        return EXIT_TIMEDOUT;
      else
        return status;
    }
}
