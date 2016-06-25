/* Process a regular text line in `line_buf'. */

static void
proc_text (void)
{
  static intmax_t blank_lines = 0;	/* Consecutive blank lines so far. */

  switch (*current_type)
    {
    case 'a':
      if (blank_join > 1)
        {
          if (1 < line_buf.length || ++blank_lines == blank_join)
            {
              print_lineno ();
              blank_lines = 0;
            }
          else
            fputs (print_no_line_fmt, stdout);
        }
      else
        print_lineno ();
      break;
    case 't':
      if (1 < line_buf.length)
        print_lineno ();
      else
        fputs (print_no_line_fmt, stdout);
      break;
    case 'n':
      fputs (print_no_line_fmt, stdout);
      break;
    case 'p':
      switch (re_search (current_regex, line_buf.buffer, line_buf.length - 1,
                         0, line_buf.length - 1, NULL))
        {
        case -2:
          error (EXIT_FAILURE, errno, _("error in regular expression search"));

        case -1:
          fputs (print_no_line_fmt, stdout);
          break;

        default:
          print_lineno ();
          break;
        }
    }
  fwrite (line_buf.buffer, sizeof (char), line_buf.length, stdout);
}

