/* Compare strings A and B as numbers without explicitly converting them to
   machine numbers.  Comparatively slow for short strings, but asymptotically
   hideously fast. */

static int
numcompare (char const *a, char const *b)
{
  while (blanks[to_uchar (*a)])
    a++;
  while (blanks[to_uchar (*b)])
    b++;

  return strnumcmp (a, b, decimal_point, thousands_sep);
}

