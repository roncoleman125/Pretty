/* Return true if *ST has been statted successfully.  */
static inline bool
cache_stat_ok (struct stat *st)
{
  return (0 <= st->st_size);
}

