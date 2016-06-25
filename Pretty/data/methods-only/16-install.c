/* Make ancestor directory DIR, whose last file name component is
   COMPONENT, with options OPTIONS.  Assume the working directory is
   COMPONENT's parent.  */
static int
make_ancestor (char const *dir, char const *component, void *options)
{
  int r = mkdir (component, DEFAULT_MODE);
  if (r == 0)
    announce_mkdir (dir, options);
  return r;
}

