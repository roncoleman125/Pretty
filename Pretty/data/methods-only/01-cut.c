static inline void
mark_range_start (size_t i)
{
  /* Record the fact that `i' is a range-start index.  */
  void *ent_from_table = hash_insert (range_start_ht, (void*) i);
  if (ent_from_table == NULL)
    {
      /* Insertion failed due to lack of memory.  */
      xalloc_die ();
    }
  assert ((size_t) ent_from_table == i);
}

