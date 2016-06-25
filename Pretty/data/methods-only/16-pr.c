/* Print one page.

   As long as there are lines left on the page and columns ready to print,
   Scan across the column list
   if the column has stored lines or the file is open
   pad to the appropriate spot
   print the column
   pad the remainder of the page with \n or \f as requested
   reset the status of all files -- any files which where on hold because
   of formfeeds are now put back into the lineup. */

static bool
print_page (void)
{
  int j;
  int lines_left_on_page;
  COLUMN *p;

  /* Used as an accumulator (with | operator) of successive values of
     pad_vertically.  The trick is to set pad_vertically
     to false before each run through the inner loop, then after that
     loop, it tells us whether a line was actually printed (whether a
     newline needs to be output -- or two for double spacing).  But those
     values have to be accumulated (in pv) so we can invoke pad_down
     properly after the outer loop completes. */
  bool pv;

  init_page ();

  if (cols_ready_to_print () == 0)
    return false;

  if (extremities)
    print_a_header = true;

  /* Don't pad unless we know a page was printed. */
  pad_vertically = false;
  pv = false;

  lines_left_on_page = lines_per_body;
  if (double_space)
    lines_left_on_page *= 2;

  while (lines_left_on_page > 0 && cols_ready_to_print () > 0)
    {
      output_position = 0;
      spaces_not_printed = 0;
      separators_not_printed = 0;
      pad_vertically = false;
      align_empty_cols = false;
      empty_line = true;

      for (j = 1, p = column_vector; j <= columns; ++j, ++p)
        {
          input_position = 0;
          if (p->lines_to_print > 0 || p->status == FF_FOUND)
            {
              FF_only = false;
              padding_not_printed = p->start_position;
              if (!(p->print_func) (p))
                read_rest_of_line (p);
              pv |= pad_vertically;

              --p->lines_to_print;
              if (p->lines_to_print <= 0)
                {
                  if (cols_ready_to_print () <= 0)
                    break;
                }

              /* File p changed its status to ON_HOLD or CLOSED */
              if (parallel_files && p->status != OPEN)
                {
                  if (empty_line)
                    align_empty_cols = true;
                  else if (p->status == CLOSED ||
                           (p->status == ON_HOLD && FF_only))
                    align_column (p);
                }
            }
          else if (parallel_files)
            {
              /* File status ON_HOLD or CLOSED */
              if (empty_line)
                align_empty_cols = true;
              else
                align_column (p);
            }

          /* We need it also with an empty column */
          if (use_col_separator)
            ++separators_not_printed;
        }

      if (pad_vertically)
        {
          putchar ('\n');
          --lines_left_on_page;
        }

      if (cols_ready_to_print () <= 0 && !extremities)
        break;

      if (double_space && pv)
        {
          putchar ('\n');
          --lines_left_on_page;
        }
    }

  if (lines_left_on_page == 0)
    for (j = 1, p = column_vector; j <= columns; ++j, ++p)
      if (p->status == OPEN)
        p->full_page_printed = true;

  pad_vertically = pv;

  if (pad_vertically && extremities)
    pad_down (lines_left_on_page + lines_per_footer);
  else if (keep_FF && print_a_FF)
    {
      putchar ('\f');
      print_a_FF = false;
    }

  if (last_page_number < page_number)
    return false;		/* Stop printing with LAST_PAGE */

  reset_status ();		/* Change ON_HOLD to OPEN. */

  return true;			/* More pages to go. */
}

