/* Optionally remove files created so far; then exit.
   Called when an error detected. */

static void
cleanup (void)
{
  sigset_t oldset;

  close_output_file ();

  sigprocmask (SIG_BLOCK, &caught_signals, &oldset);
  delete_all_files (false);
  sigprocmask (SIG_SETMASK, &oldset, NULL);
}

static void cleanup_fatal (void) ATTRIBUTE_NORETURN;
