/* Install the signal handlers.  */

static void
install_signal_handlers (void)
{
  bool catch_siginfo = ! (SIGINFO == SIGUSR1 && getenv ("POSIXLY_CORRECT"));


  struct sigaction act;
  sigemptyset (&caught_signals);
  if (catch_siginfo)
    {
      sigaction (SIGINFO, NULL, &act);
      if (act.sa_handler != SIG_IGN)
        sigaddset (&caught_signals, SIGINFO);
    }
  sigaction (SIGINT, NULL, &act);
  if (act.sa_handler != SIG_IGN)
    sigaddset (&caught_signals, SIGINT);
  act.sa_mask = caught_signals;

  if (sigismember (&caught_signals, SIGINFO))
    {
      act.sa_handler = siginfo_handler;
      act.sa_flags = 0;
      sigaction (SIGINFO, &act, NULL);
    }

  if (sigismember (&caught_signals, SIGINT))
    {
      /* POSIX 1003.1-2001 says SA_RESETHAND implies SA_NODEFER,
         but this is not true on Solaris 8 at least.  It doesn't
         hurt to use SA_NODEFER here, so leave it in.  */
      act.sa_handler = interrupt_handler;
      act.sa_flags = SA_NODEFER | SA_RESETHAND;
      sigaction (SIGINT, &act, NULL);
    }


  if (catch_siginfo && signal (SIGINFO, SIG_IGN) != SIG_IGN)
    {
      signal (SIGINFO, siginfo_handler);
      siginterrupt (SIGINFO, 1);
    }
  if (signal (SIGINT, SIG_IGN) != SIG_IGN)
    {
      signal (SIGINT, interrupt_handler);
      siginterrupt (SIGINT, 1);
    }
}

