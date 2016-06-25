/* Initialize a fstatat cache *ST.  Return ST for convenience.  */
static inline struct stat *
cache_stat_init (struct stat *st)
{
  st->st_size = -1;
  return st;
}

