static void
overwrite_prompt (char const *dst_name, struct stat const *dst_sb)
{
  if (! writable_destination (dst_name, dst_sb->st_mode))
    {
      char perms[12];		/* "-rwxrwxrwx " ls-style modes. */
      strmode (dst_sb->st_mode, perms);
      perms[10] = '\0';
      fprintf (stderr,
               _("%s: try to overwrite %s, overriding mode %04lo (%s)? "),
               program_name, quote (dst_name),
               (unsigned long int) (dst_sb->st_mode & CHMOD_MODE_BITS),
               &perms[1]);
    }
  else
    {
      fprintf (stderr, _("%s: overwrite %s? "),
               program_name, quote (dst_name));
    }
}

/* Initialize the hash table implementing a set of F_triple entries
   corresponding to destination files.  */
extern void
dest_info_init (struct cp_options *x)
{
  x->dest_info
    = hash_initialize (DEST_INFO_INITIAL_CAPACITY,
                       NULL,
                       triple_hash,
                       triple_compare,
                       triple_free);
}

/* Initialize the hash table implementing a set of F_triple entries
   corresponding to source files listed on the command line.  */
extern void
src_info_init (struct cp_options *x)
{

  /* Note that we use triple_hash_no_name here.
     Contrast with the use of triple_hash above.
     That is necessary because a source file may be specified
     in many different ways.  We want to warn about this
       cp a a d/
     as well as this:
       cp a ./a d/
  */
  x->src_info
    = hash_initialize (DEST_INFO_INITIAL_CAPACITY,
                       NULL,
                       triple_hash_no_name,
                       triple_compare,
                       triple_free);
}

