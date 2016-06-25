/* Reap all children, waiting if necessary.  */

static void
reap_all (void)
{
  while (0 < nprocs)
    reap (-1);
}

