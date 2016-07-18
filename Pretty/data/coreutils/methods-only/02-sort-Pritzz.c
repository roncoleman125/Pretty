/* Enter a critical section.  */
static struct cs_status
cs_enter (void)
{
  struct cs_status status;
  status.valid = (sigprocmask (SIG_BLOCK, &caught_signals, &status.sigs) == 0);
  return status;
}

