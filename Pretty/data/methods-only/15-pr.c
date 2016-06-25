/* Align empty columns and print separators.
   Empty columns will be formed by files with status ON_HOLD or CLOSED
   when printing multiple files in parallel. */

static void
align_column (COLUMN *p)
{
  padding_not_printed = p->start_position;
  if (padding_not_printed - col_sep_length > 0)
    {
      pad_across_to (padding_not_printed - col_sep_length);
      padding_not_printed = ANYWHERE;
    }

  if (use_col_separator)
    print_sep_string ();

  if (p->numbered)
    add_line_number (p);
}

