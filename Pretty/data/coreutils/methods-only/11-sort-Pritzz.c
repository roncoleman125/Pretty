/* Reap at least one exited child, waiting if necessary.  */

static void
reap_some (void)
{
  reap (-1);
  reap_exited ();
}

