/* Pop a dev/ino struct off the global dev_ino_obstack
   and return that struct.  */
static struct dev_ino
dev_ino_pop (void)
{
  assert (sizeof (struct dev_ino) <= obstack_object_size (&dev_ino_obstack));
  obstack_blank (&dev_ino_obstack, -(int) (sizeof (struct dev_ino)));
/* Pop a dev/ino struct off the global dev_ino_obstack
   and return that struct.  */
  return *(struct dev_ino *) obstack_next_free (&dev_ino_obstack);
}


/* Note the use commented out below:
  do						\
    {						\
      struct stat sb;				\
      assert (Name);				\
      assert (0 <= stat (Name, &sb));		\
      assert (sb.st_dev == Di.st_dev);		\
      assert (sb.st_ino == Di.st_ino);		\
    }						\
  while (0)
*/

