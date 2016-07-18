/* Avoid closing stdout twice.  Since we conditionally call
   close_stream (stdout) in order to decide whether to clean up a
   temporary file, the exit hook needs to know whether to do all of
   close_stdout or just the stderr half.  */
static void
maybe_close_stdout (void)
{
  if (!stdout_closed)
    close_stdout ();
  else if (close_stream (stderr) != 0)
    _exit (EXIT_FAILURE);
}

