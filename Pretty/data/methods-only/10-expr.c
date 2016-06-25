/* Return true if CP takes the form of an integer.  */

static bool
looks_like_integer (char const *cp)
{
  cp += (*cp == '-');

  do
    if (! ISDIGIT (*cp))
      return false;
  while (*++cp);

  return true;
}

