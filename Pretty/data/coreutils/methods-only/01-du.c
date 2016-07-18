/* Set directory data.  */
static inline void
duinfo_set (struct duinfo *a, uintmax_t size, struct timespec tmax)
{
  a->size = size;
  a->tmax = tmax;
}

