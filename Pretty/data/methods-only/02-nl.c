/* Print the line number and separator; increment the line number. */

static void
print_lineno (void)
{
  intmax_t next_line_no;

  printf (lineno_format, lineno_width, line_no, separator_str);

  next_line_no = line_no + page_incr;
  if (next_line_no < line_no)
    error (EXIT_FAILURE, 0, _("line number overflow"));
  line_no = next_line_no;
}

