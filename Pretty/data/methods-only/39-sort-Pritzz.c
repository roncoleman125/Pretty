/* Compare numbers A and B ending in units with SI or IEC prefixes
       <none/unknown> < K/k < M < G < T < P < E < Z < Y  */

static int
human_numcompare (char const *a, char const *b)
{
  while (blanks[to_uchar (*a)])
    a++;
  while (blanks[to_uchar (*b)])
    b++;

  int diff = find_unit_order (a) - find_unit_order (b);
  return (diff ? diff : strnumcmp (a, b, decimal_point, thousands_sep));
}

