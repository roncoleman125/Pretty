/* Compute the next line number.  */

static void
next_line_num (void)
{
  char *endp = line_num_end;
  do
    {
      if ((*endp)++ < '9')
        return;
      *endp-- = '0';
    }
  while (endp >= line_num_start);
  if (line_num_start > line_buf)
    *--line_num_start = '1';
  else
    *line_buf = '>';
  if (line_num_start < line_num_print)
    line_num_print--;
}

