static void
add_line_number (COLUMN *p)
{
  int i;
  char *s;
  int left_cut;

  /* Cutting off the higher-order digits is more informative than
     lower-order cut off*/
  if (line_number < power_10)
    sprintf (number_buff, "%*d", chars_per_number, line_number);
  else
    {
      left_cut = line_number % power_10;
      sprintf (number_buff, "%0*d", chars_per_number, left_cut);
    }
  line_number++;
  s = number_buff;
  for (i = chars_per_number; i > 0; i--)
    (p->char_func) (*s++);

  if (columns > 1)
    {
      /* Tabification is assumed for multiple columns, also for n-separators,
         but `default n-separator = TAB' hasn't been given priority over
         equal column_width also specified by POSIX. */
      if (number_separator == '\t')
        {
          i = number_width - chars_per_number;
          while (i-- > 0)
            (p->char_func) (' ');
        }
      else
        (p->char_func) (number_separator);
    }
  else
    /* To comply with POSIX, we avoid any expansion of default TAB
       separator with a single column output. No column_width requirement
       has to be considered. */
    {
      (p->char_func) (number_separator);
      if (number_separator == '\t')
        output_position = POS_AFTER_TAB (chars_per_output_tab,
                          output_position);
    }

  if (truncate_lines && !parallel_files)
    input_position += number_width;
}

