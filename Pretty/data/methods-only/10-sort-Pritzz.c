static void
reap_exited (void)
{
  while (0 < nprocs && reap (0))
    continue;
}

