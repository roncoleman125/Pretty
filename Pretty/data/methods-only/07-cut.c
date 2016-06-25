/* Return nonzero if the K'th field or byte is printable.
   When returning nonzero, if RANGE_START is non-NULL,
   set *RANGE_START to true if K is the beginning of a range, and to
   false otherwise.  */

static bool
print_kth (size_t k, bool *range_start)
{
  bool k_selected
    = ((0 < eol_range_start && eol_range_start <= k)
       || (k <= max_range_endpoint && is_printable_field (k)));

  bool is_selected = k_selected ^ complement;
  if (range_start && is_selected)
    *range_start = is_range_start_index (k);

  return is_selected;
}

