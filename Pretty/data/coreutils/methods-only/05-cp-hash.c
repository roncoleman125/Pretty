/* Add file NAME, copied from inode number INO and device number DEV,
   to the list of files we have copied.
   Return NULL if inserted, otherwise non-NULL. */

extern char *
remember_copied (const char *name, ino_t ino, dev_t dev)
{
  struct Src_to_dest *ent;
  struct Src_to_dest *ent_from_table;

  ent = xmalloc (sizeof *ent);
  ent->name = xstrdup (name);
  ent->st_ino = ino;
  ent->st_dev = dev;

  ent_from_table = hash_insert (src_to_dest, ent);
  if (ent_from_table == NULL)
    {
      /* Insertion failed due to lack of memory.  */
      xalloc_die ();
    }

  /* Determine whether there was already an entry in the table
     with a matching key.  If so, free ENT (it wasn't inserted) and
     return the `name' from the table entry.  */
  if (ent_from_table != ent)
    {
      src_to_dest_free (ent);
      return (char *) ent_from_table->name;
    }

  /* New key;  insertion succeeded.  */
  return NULL;
}

