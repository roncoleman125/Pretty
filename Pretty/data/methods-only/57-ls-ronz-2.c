/* List all the files now in the table.  */

static void
print_current_files (void)
{
  size_t i;

  switch (format)
    {
    case one_per_line:
      for (i = 0; i < cwd_n_used; i++)
        {
          print_file_name_and_frills (sorted_file[i], 0);
          putchar ('\n');
        }
      break;

    case many_per_line:
      print_many_per_line ();
      break;

    case horizontal:
      print_horizontal ();
      break;

    case with_commas:
      print_with_commas ();
      break;

    case long_format:
      for (i = 0; i < cwd_n_used; i++)
        {
          set_normal_color ();
          print_long_format (sorted_file[i]);
          DIRED_PUTCHAR ('\n');
        }
      break;
    }
}

