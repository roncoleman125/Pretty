/* Initialize directory data.  */
static inline void
duinfo_init (struct duinfo *a)
{
  a->size = 0;
  a->tmax.tv_sec = TYPE_MINIMUM (time_t);
  a->tmax.tv_nsec = -1;
}

