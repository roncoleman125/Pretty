/* Make ancestor directory DIR, whose last component is COMPONENT,
   with options OPTIONS.  Assume the working directory is COMPONENT's
   parent.  Return 0 if successful and the resulting directory is
   readable, 1 if successful but the resulting directory is not
   readable, -1 (setting errno) otherwise.  */
static int
make_ancestor (char const *dir, char const *component, void *options)
{
  struct mkdir_options const *o = options;
  int r = mkdir (component, o->ancestor_mode);
  if (r == 0)
    {
      r = ! (o->ancestor_mode & S_IRUSR);
      announce_mkdir (dir, options);
    }
  return r;
}

