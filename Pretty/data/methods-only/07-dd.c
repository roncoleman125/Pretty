/* An ordinary signal was received; arrange for the program to exit.  */

static void
interrupt_handler (int sig)
{
  if (! SA_RESETHAND)
    signal (sig, SIG_DFL);
  interrupt_signal = sig;
}

