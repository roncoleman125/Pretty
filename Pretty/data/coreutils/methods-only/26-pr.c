/* Print column separators.

   We keep a count until we know that we'll be printing a line,
   then print_sep_string() is called. */

static void
print_sep_string (void)
{
  char *s;
  int l = col_sep_length;

  s = col_sep_string;

  if (separators_not_printed <= 0)
    {
      /* We'll be starting a line with chars_per_margin, anything else? */
      if (spaces_not_printed > 0)
        print_white_space ();
    }
  else
    {
      for (; separators_not_printed > 0; --separators_not_printed)
        {
          while (l-- > 0)
            {
              /* 3 types of sep_strings: spaces only, spaces and chars,
              chars only */
              if (*s == ' ')
                {
                  /* We're tabifying output; consecutive spaces in
                  sep_string may have to be converted to tabs */
                  s++;
                  ++spaces_not_printed;
                }
              else
                {
                  if (spaces_not_printed > 0)
                    print_white_space ();
                  putchar (*s++);
                  ++output_position;
                }
            }
          /* sep_string ends with some spaces */
          if (spaces_not_printed > 0)
            print_white_space ();
        }
    }
}

