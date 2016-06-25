/* Return the name of output file number NUM.

   This function is called from a signal handler, so it should invoke
   only reentrant functions that are async-signal-safe.  POSIX does
   not guarantee this for the functions called below, but we don't
   know of any hosts where this implementation isn't safe.  */

static char *
make_filename (unsigned int num)
{
  strcpy (filename_space, prefix);
  if (suffix)
    sprintf (filename_space + strlen (prefix), suffix, num);
  else
    sprintf (filename_space + strlen (prefix), "%0*u", digits, num);
  return filename_space;
}

