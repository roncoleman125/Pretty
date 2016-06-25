/* Map a TS with negative TS.tv_nsec to {0,0}.  */
static inline struct timespec
neg_to_zero (struct timespec ts)
{
  if (0 <= ts.tv_nsec)
    return ts;
  struct timespec z = {0, 0};
  return z;
}

