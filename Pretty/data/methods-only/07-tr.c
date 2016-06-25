/* Return a newly allocated string with a printable version of C.
   This function is used solely for formatting error messages.  */

static char *
make_printable_char (unsigned char c)
{
  char *buf = xmalloc (5);

  if (isprint (c))
    {
      buf[0] = c;
      buf[1] = '\0';
    }
  else
    {
      sprintf (buf, "\\%03o", c);
    }
  return buf;
}

