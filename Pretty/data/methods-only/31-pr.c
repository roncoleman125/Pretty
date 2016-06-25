/* Print (or store, if p->char_func is store_char()) a line.

   Read a character to determine whether we have a line or not.
   (We may hit EOF, \n, or \f)

   Once we know we have a line,
   set pad_vertically = true, meaning it's safe
   to pad down at the end of the page, since we do have a page.
   print a header if needed.
   pad across to padding_not_printed if needed.
   print any separators which need to be printed.
   print a line number if it needs to be printed.

   Print the clump which corresponds to the first character.

   Enter a loop and keep printing until an end of line condition
   exists, or until we exceed chars_per_column.

   Return false if we exceed chars_per_column before reading
   an end of line character, true otherwise. */

static bool
read_line (COLUMN *p)
{
  int c;
  int chars IF_LINT ( = 0);
  int last_input_position;
  int j, k;
  COLUMN *q;

  /* read 1st character in each line or any character succeeding a FF: */
  c = getc (p->fp);

  last_input_position = input_position;

  if (c == '\f' && p->full_page_printed)
    if ((c = getc (p->fp)) == '\n')
      c = getc (p->fp);
  p->full_page_printed = false;

  switch (c)
    {
    case '\f':
      if ((c = getc (p->fp)) != '\n')
        ungetc (c, p->fp);
      FF_only = true;
      if (print_a_header && !storing_columns)
        {
          pad_vertically = true;
          print_header ();
        }
      else if (keep_FF)
        print_a_FF = true;
      hold_file (p);
      return true;
    case EOF:
      close_file (p);
      return true;
    case '\n':
      break;
    default:
      chars = char_to_clump (c);
    }

  if (truncate_lines && input_position > chars_per_column)
    {
      input_position = last_input_position;
      return false;
    }

  if (p->char_func != store_char)
    {
      pad_vertically = true;

      if (print_a_header && !storing_columns)
        print_header ();

      if (parallel_files && align_empty_cols)
        {
          /* We have to align empty columns at the beginning of a line. */
          k = separators_not_printed;
          separators_not_printed = 0;
          for (j = 1, q = column_vector; j <= k; ++j, ++q)
            {
              align_column (q);
              separators_not_printed += 1;
            }
          padding_not_printed = p->start_position;
          if (truncate_lines)
            spaces_not_printed = chars_per_column;
          else
            spaces_not_printed = 0;
          align_empty_cols = false;
        }

      if (padding_not_printed - col_sep_length > 0)
        {
          pad_across_to (padding_not_printed - col_sep_length);
          padding_not_printed = ANYWHERE;
        }

      if (use_col_separator)
        print_sep_string ();
    }

  if (p->numbered)
    add_line_number (p);

  empty_line = false;
  if (c == '\n')
    return true;

  print_clump (p, chars, clump_buff);

  while (true)
    {
      c = getc (p->fp);

      switch (c)
        {
        case '\n':
          return true;
        case '\f':
          if ((c = getc (p->fp)) != '\n')
            ungetc (c, p->fp);
          if (keep_FF)
            print_a_FF = true;
          hold_file (p);
          return true;
        case EOF:
          close_file (p);
          return true;
        }

      last_input_position = input_position;
      chars = char_to_clump (c);
      if (truncate_lines && input_position > chars_per_column)
        {
          input_position = last_input_position;
          return false;
        }

      print_clump (p, chars, clump_buff);
    }
}


