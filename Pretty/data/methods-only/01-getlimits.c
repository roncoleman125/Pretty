/* Add one to the absolute value of the number whose textual
   representation is BUF + 1.  Do this in-place, in the buffer.
   Return a pointer to the result, which is normally BUF + 1, but is
   BUF if the representation grew in size.  */
static char const *
decimal_absval_add_one (char *buf)
{
  bool negative = (buf[1] == '-');
  char *absnum = buf + 1 + negative;
  char *p = absnum + strlen (absnum);
  absnum[-1] = '0';
  while (*--p == '9')
    *p = '0';
  ++*p;
  char *result = MIN (absnum, p);
  if (negative)
    *--result = '-';
  return result;
}

