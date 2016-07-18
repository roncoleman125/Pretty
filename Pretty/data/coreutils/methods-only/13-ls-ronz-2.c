/* Process any pending signals.  If signals are caught, this function
   should be called periodically.  Ideally there should never be an
   unbounded amount of time when signals are not being processed.
   Signal handling can restore the default colors, so callers must
   immediately change colors after invoking this function.  */

static void
process_signals (void)
{
  while (interrupt_signal || stop_signal_count)
    {
      int sig;
      int stops;
      sigset_t oldset;

      if (used_color)
        restore_default_color ();
      fflush (stdout);

      sigprocmask (SIG_BLOCK, &caught_signals, &oldset);

      /* Reload interrupt_signal and stop_signal_count, in case a new
         signal was handled before sigprocmask took effect.  */
      sig = interrupt_signal;
      stops = stop_signal_count;

      /* SIGTSTP is special, since the application can receive that signal
         more than once.  In this case, don't set the signal handler to the
         default.  Instead, just raise the uncatchable SIGSTOP.  */
      if (stops)
        {
          stop_signal_count = stops - 1;
          sig = SIGSTOP;
        }
      else
        signal (sig, SIG_DFL);

      /* Exit or suspend the program.  */
      raise (sig);
      sigprocmask (SIG_SETMASK, &oldset, NULL);

      /* If execution reaches here, then the program has been
         continued (after being suspended).  */
    }
}

