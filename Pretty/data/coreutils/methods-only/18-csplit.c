/* Return true if at least one more line is available for input. */

static bool
no_more_lines (void)
{
  return find_line (current_line + 1) == NULL;
}

