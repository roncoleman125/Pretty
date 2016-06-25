/* An info signal was received; arrange for the program to print status.  */

static void
siginfo_handler (int sig)
{
  if (! SA_NOCLDSTOP)
    signal (sig, siginfo_handler);
  info_signal_count++;
}

