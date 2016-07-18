static bool
head (const char *filename, int fd, uintmax_t n_units, bool count_lines,
      bool elide_from_end)
{
  if (print_headers)
    write_header (filename);

  if (elide_from_end)
    {
      if (count_lines)
        {
          return elide_tail_lines_file (filename, fd, n_units);
        }
      else
        {
          return elide_tail_bytes_file (filename, fd, n_units);
        }
    }
  if (count_lines)
    return head_lines (filename, fd, n_units);
  else
    return head_bytes (filename, fd, n_units);
}

