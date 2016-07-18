/* Initialize the hash table.  */
extern void
hash_init (void)
{
  src_to_dest = hash_initialize (INITIAL_TABLE_SIZE, NULL,
                                 src_to_dest_hash,
                                 src_to_dest_compare,
                                 src_to_dest_free);
  if (src_to_dest == NULL)
    xalloc_die ();
}

