/* Return true if a line whose first non-blank character after the
   prefix (if any) is C could belong to the current paragraph,
   otherwise false.  */

static bool
same_para (int c)
{
  return (next_prefix_indent == prefix_indent
          && in_column >= next_prefix_indent + prefix_full_length
          && c != '\n' && c != EOF);
}

