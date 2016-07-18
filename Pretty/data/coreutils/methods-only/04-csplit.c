/* Keep track of NUM bytes of a partial line in buffer START.
   These bytes will be retrieved later when another large buffer is read.  */

static void
save_to_hold_area (char *start, size_t num)
{
  free (hold_area);
  hold_area = start;
  hold_count = num;
}

