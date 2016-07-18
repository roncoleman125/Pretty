static size_t
src_to_dest_hash (void const *x, size_t table_size)
{
  struct Src_to_dest const *p = x;

  /* Ignoring the device number here should be fine.  */
  /* The cast to uintmax_t prevents negative remainders
     if st_ino is negative.  */
  return (uintmax_t) p->st_ino % table_size;
}

