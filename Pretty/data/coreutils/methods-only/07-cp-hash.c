/* Reset the hash structure in the global variable `htab' to
   contain no entries.  */

extern void
forget_all (void)
{
  hash_free (src_to_dest);
}
