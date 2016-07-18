/* Same as stream_open, except always return a non-null value; die on
   failure.  */

static FILE *
xfopen (char const *file, char const *how)
{
  FILE *fp = stream_open (file, how);
  if (!fp)
    die (_("open failed"), file);
  return fp;
}

