static void
print_with_commas (void)
{
  size_t filesno;
  size_t pos = 0;

  for (filesno = 0; filesno < cwd_n_used; filesno++)
    {
      struct fileinfo const *f = sorted_file[filesno];
      size_t len = length_of_file_name_and_frills (f);

      if (filesno != 0)
        {
          char separator;

          if (pos + len + 2 < line_length)
            {
              pos += 2;
              separator = ' ';
            }
          else
            {
              pos = 0;
              separator = '\n';
            }

          putchar (',');
          putchar (separator);
        }

      print_file_name_and_frills (f, pos);
      pos += len;
    }
  putchar ('\n');
}

