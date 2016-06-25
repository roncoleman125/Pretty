/* Determine print_func and char_func, the functions
   used by each column for printing and/or storing.

   Determine the horizontal position desired when we begin
   printing a column (p->start_position). */

static void
init_funcs (void)
{
  int i, h, h_next;
  COLUMN *p;

  h = chars_per_margin;

  if (!truncate_lines)
    h_next = ANYWHERE;
  else
    {
      /* When numbering lines of parallel files, we enlarge the
         first column to accomodate the number.  Looks better than
         the Sys V approach. */
      if (parallel_files && numbered_lines)
        h_next = h + chars_per_column + number_width;
      else
        h_next = h + chars_per_column;
    }

  /* Enlarge p->start_position of first column to use the same form of
     padding_not_printed with all columns. */
  h = h + col_sep_length;

  /* This loop takes care of all but the rightmost column. */

  for (p = column_vector, i = 1; i < columns; ++p, ++i)
    {
      if (storing_columns)	/* One file, multi columns down. */
        {
          p->char_func = store_char;
          p->print_func = print_stored;
        }
      else
        /* One file, multi columns across; or parallel files.  */
        {
          p->char_func = print_char;
          p->print_func = read_line;
        }

      /* Number only the first column when printing files in
         parallel. */
      p->numbered = numbered_lines && (!parallel_files || i == 1);
      p->start_position = h;

      /* If we don't truncate lines, all start_positions are
         ANYWHERE, except the first column's start_position when
         using a margin. */

      if (!truncate_lines)
        {
          h = ANYWHERE;
          h_next = ANYWHERE;
        }
      else
        {
          h = h_next + col_sep_length;
          h_next = h + chars_per_column;
        }
    }

  /* The rightmost column.

     Doesn't need to be stored unless we intend to balance
     columns on the last page. */
  if (storing_columns && balance_columns)
    {
      p->char_func = store_char;
      p->print_func = print_stored;
    }
  else
    {
      p->char_func = print_char;
      p->print_func = read_line;
    }

  p->numbered = numbered_lines && (!parallel_files || i == 1);
  p->start_position = h;
}

