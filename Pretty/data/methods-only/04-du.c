/* Try to insert the INO/DEV pair into the global table, HTAB.
   Return true if the pair is successfully inserted,
   false if the pair is already in the table.  */
static bool
hash_ins (ino_t ino, dev_t dev)
{
  int inserted = di_set_insert (di_set, dev, ino);
  if (inserted < 0)
    xalloc_die ();
  return inserted;
}

