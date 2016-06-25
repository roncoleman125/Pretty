/* Return the start of the next line after LINE.  The current line
   ends in EOLBYTE, and is guaranteed to end before LINE + N.  */

static char *
next_line (char *line, char eolbyte, size_t n)
{
  char *p = memchr (line, eolbyte, n);
  return p + 1;
}

