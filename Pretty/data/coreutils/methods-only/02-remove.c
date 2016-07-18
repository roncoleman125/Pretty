/* Return true if *ST has been statted.  */
static inline bool
cache_statted (struct stat *st)
{
  return (st->st_size != -1);
}

