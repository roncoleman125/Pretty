/* Cleanup actions to take when exiting.  */

static void
exit_cleanup (void)
{
  if (temphead)
    {
      /* Clean up any remaining temporary files in a critical section so
         that a signal handler does not try to clean them too.  */
      struct cs_status cs = cs_enter ();
      cleanup ();
      cs_leave (cs);
    }

  close_stdout ();
}

