/* Create the next output file. */

static void
create_output_file (void)
{
  bool fopen_ok;
  int fopen_errno;

  output_filename = make_filename (files_created);

  if (files_created == UINT_MAX)
    {
      fopen_ok = false;
      fopen_errno = EOVERFLOW;
    }
  else
    {
      /* Create the output file in a critical section, to avoid races.  */
      sigset_t oldset;
      sigprocmask (SIG_BLOCK, &caught_signals, &oldset);
      output_stream = fopen (output_filename, "w");
      fopen_ok = (output_stream != NULL);
      fopen_errno = errno;
      files_created += fopen_ok;
      sigprocmask (SIG_SETMASK, &oldset, NULL);
    }

  if (! fopen_ok)
    {
      error (0, fopen_errno, "%s", output_filename);
      cleanup_fatal ();
    }
  bytes_written = 0;
}

