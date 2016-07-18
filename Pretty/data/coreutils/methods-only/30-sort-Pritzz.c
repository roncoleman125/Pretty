/* Return the default sort size.  */
static size_t
default_sort_size (void)
{
  /* Let MEM be available memory or 1/8 of total memory, whichever
     is greater.  */
  double avail = physmem_available ();
  double total = physmem_total ();
  double mem = MAX (avail, total / 8);
  struct rlimit rlimit;

  /* Let SIZE be MEM, but no more than the maximum object size or
     system resource limits.  Avoid the MIN macro here, as it is not
     quite right when only one argument is floating point.  Don't
     bother to check for values like RLIM_INFINITY since in practice
     they are not much less than SIZE_MAX.  */
  size_t size = SIZE_MAX;
  if (mem < size)
    size = mem;
  if (getrlimit (RLIMIT_DATA, &rlimit) == 0 && rlimit.rlim_cur < size)
    size = rlimit.rlim_cur;
  if (getrlimit (RLIMIT_AS, &rlimit) == 0 && rlimit.rlim_cur < size)
    size = rlimit.rlim_cur;

  /* Leave a large safety margin for the above limits, as failure can
     occur when they are exceeded.  */
  size /= 2;

  /* Leave a 1/16 margin for RSS to leave room for code, stack, etc.
     Exceeding RSS is not fatal, but can be quite slow.  */
  if (getrlimit (RLIMIT_RSS, &rlimit) == 0 && rlimit.rlim_cur / 16 * 15 < size)
    size = rlimit.rlim_cur / 16 * 15;

  /* Use no less than the minimum.  */
  return MAX (size, MIN_SORT_SIZE);
}

