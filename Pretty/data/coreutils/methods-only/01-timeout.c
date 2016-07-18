static void
cleanup (int sig)
{
  if (sig == SIGALRM)
    {
      timed_out = 1;
      sig = term_signal;
    }
  if (monitored_pid)
    {
      if (sigs_to_ignore[sig])
        {
          sigs_to_ignore[sig] = 0;
          return;
        }
      if (kill_after)
        {
          /* Start a new timeout after which we'll send SIGKILL.  */
          term_signal = SIGKILL;
          alarm (kill_after);
          kill_after = 0; /* Don't let later signals reset kill alarm.  */
        }
      send_sig (0, sig);
      if (sig != SIGKILL && sig != SIGCONT)
        send_sig (0, SIGCONT);
    }
  else /* we're the child or the child is not exec'd yet.  */
    _exit (128 + sig);
}

