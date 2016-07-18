static void
print_many_per_line (void)
{
  size_t row;			/* Current row.  */
  size_t cols = calculate_columns (true);
  struct column_info const *line_fmt = &column_info[cols - 1];

  /* Calculate the number of rows that will be in each column except possibly
     for a short column on the right.  */
  size_t rows = cwd_n_used / cols + (cwd_n_used % cols != 0);

  for (row = 0; row < rows; row++)
    {
      size_t col = 0;
      size_t filesno = row;
      size_t pos = 0;

      /* Print the next row.  */
      while (1)
        {
          struct fileinfo const *f = sorted_file[filesno];
          size_t name_length = length_of_file_name_and_frills (f);
          size_t max_name_length = line_fmt->col_arr[col++];
          print_file_name_and_frills (f, pos);

          filesno += rows;
          if (filesno >= cwd_n_used)
            break;

          indent (pos + name_length, pos + max_name_length);
          pos += max_name_length;
        }
      putchar ('\n');
    }
}

