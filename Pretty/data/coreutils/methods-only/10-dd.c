/* Process any pending signals.  If signals are caught, this function
   should be called periodically.  Ideally there should never be an
   unbounded amount of time when signals are not being processed.  */

static void
process_signals (void)
{
  while (interrupt_signal || info_signal_count)
    {
      int interrupt;
      int infos;
      sigset_t oldset;

      sigprocmask (SIG_BLOCK, &caught_signals, &oldset);

      /* Reload interrupt_signal and info_signal_count, in case a new
         signal was handled before sigprocmask took effect.  */
      interrupt = interrupt_signal;
      infos = info_signal_count;

      if (infos)
        info_signal_count = infos - 1;

      sigprocmask (SIG_SETMASK, &oldset, NULL);

      if (interrupt)
        cleanup ();
      print_stats ();
      if (interrupt)
        raise (interrupt);
    }
}

