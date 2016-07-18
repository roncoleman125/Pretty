/* Put DIRNAME/NAME into DEST, handling `.' and `/' properly.  */
/* FIXME: maybe remove this function someday.  See about using a
   non-malloc'ing version of file_name_concat.  */

static void
attach (char *dest, const char *dirname, const char *name)
{
  const char *dirnamep = dirname;

  /* Copy dirname if it is not ".".  */
  if (dirname[0] != '.' || dirname[1] != 0)
    {
      while (*dirnamep)
        *dest++ = *dirnamep++;
      /* Add '/' if `dirname' doesn't already end with it.  */
      if (dirnamep > dirname && dirnamep[-1] != '/')
        *dest++ = '/';
    }
  while (*name)
    *dest++ = *name++;
  *dest = 0;
}

