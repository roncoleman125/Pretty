/* Handle interrupts and hangups. */

static void
sighandler (int sig)
{
  if (! SA_NOCLDSTOP)
    signal (sig, SIG_IGN);

  cleanup ();

  signal (sig, SIG_DFL);
  raise (sig);
}

