/* Leave a critical section.  */
static void
cs_leave (struct cs_status status)
{
  if (status.valid)
    {
      /* Ignore failure when restoring the signal mask. */
      sigprocmask (SIG_SETMASK, &status.sigs, NULL);
    }
}

