/* Convert a null-terminated (possibly zero-length) string S to an
   unsigned long integer value.  If S points to a non-digit set *P to S,
   *VAL to 0, and return true.  Otherwise, accumulate the integer value of
   the string of digits.  If the string of digits represents a value
   larger than ULONG_MAX, don't modify *VAL or *P and return false.
   Otherwise, advance *P to the first non-digit after S, set *VAL to
   the result of the conversion and return true.  */

static bool
simple_strtoul (const char *s, const char **p, unsigned long int *val)
{
  unsigned long int sum;

  sum = 0;
  while (ISDIGIT (*s))
    {
      int c = *s++ - '0';
      if (sum > (ULONG_MAX - c) / 10)
        return false;
      sum = sum * 10 + c;
    }
  *p = s;
  *val = sum;
  return true;
}

