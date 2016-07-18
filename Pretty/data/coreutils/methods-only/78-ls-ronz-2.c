static void
print_horizontal (void)
{
  size_t filesno;
  size_t pos = 0;
  size_t cols = calculate_columns (false);
  struct column_info const *line_fmt = &column_info[cols - 1];
  struct fileinfo const *f = sorted_file[0];
  size_t name_length = length_of_file_name_and_frills (f);
  size_t max_name_length = line_fmt->col_arr[0];

  /* Print first entry.  */
  print_file_name_and_frills (f, 0);

  /* Now the rest.  */
  for (filesno = 1; filesno < cwd_n_used; ++filesno)
    {
      size_t col = filesno % cols;

      if (col == 0)
        {
          putchar ('\n');
          pos = 0;
        }
      else
        {
          indent (pos + name_length, pos + max_name_length);
          pos += max_name_length;
        }

      f = sorted_file[filesno];
      print_file_name_and_frills (f, pos);

      name_length = length_of_file_name_and_frills (f);
      max_name_length = line_fmt->col_arr[col];
    }
  putchar ('\n');
}

