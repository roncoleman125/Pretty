/* Return an integer that represents the order of magnitude of the
   unit following the number.  The number may contain thousands
   separators and a decimal point, but it may not contain leading blanks.
   Negative numbers get negative orders; zero numbers have a zero order.  */

static int
find_unit_order (char const *number)
{
  bool minus_sign = (*number == '-');
  char const *p = number + minus_sign;
  int nonzero = 0;
  unsigned char ch;

  /* Scan to end of number.
     Decimals or separators not followed by digits stop the scan.
     Numbers ending in decimals or separators are thus considered
     to be lacking in units.
     FIXME: add support for multibyte thousands_sep and decimal_point.  */

  do
    {
      while (ISDIGIT (ch = *p++))
        nonzero |= ch - '0';
    }
  while (ch == thousands_sep);

  if (ch == decimal_point)
    while (ISDIGIT (ch = *p++))
      nonzero |= ch - '0';

  if (nonzero)
    {
      int order = unit_order[ch];
      return (minus_sign ? -order : order);
    }
  else
    return 0;
}

