/* Remove the entry matching INO/DEV from the table
   that maps source ino/dev to destination file name.  */
extern void
forget_created (ino_t ino, dev_t dev)
{
  struct Src_to_dest probe;
  struct Src_to_dest *ent;

  probe.st_ino = ino;
  probe.st_dev = dev;
  probe.name = NULL;

  ent = hash_delete (src_to_dest, &probe);
  if (ent)
    src_to_dest_free (ent);
}

