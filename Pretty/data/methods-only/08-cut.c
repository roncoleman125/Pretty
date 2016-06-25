/* Comparison function for qsort to order the list of
   struct range_pairs.  */
static int
compare_ranges (const void *a, const void *b)
{
  int a_start = ((const struct range_pair *) a)->lo;
  int b_start = ((const struct range_pair *) b)->lo;
  return a_start < b_start ? -1 : a_start > b_start;
}

