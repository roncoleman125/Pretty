/* If INO/DEV correspond to an already-copied source file, return the
   name of the corresponding destination file.  Otherwise, return NULL.  */

extern char *
src_to_dest_lookup (ino_t ino, dev_t dev)
{
  struct Src_to_dest ent;
  struct Src_to_dest const *e;
  ent.st_ino = ino;
  ent.st_dev = dev;
  e = hash_lookup (src_to_dest, &ent);
  return e ? e->name : NULL;
}

