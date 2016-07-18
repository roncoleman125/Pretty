static void
interrupt_handler (int sig)
{
  delete_all_files (true);
  /* The signal has been reset to SIG_DFL, but blocked during this
     handler.  Force the default action of this signal once the
     handler returns and the block is removed.  */
  raise (sig);
}

