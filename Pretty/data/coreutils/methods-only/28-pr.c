/* Print a character.

   Update the following comment: process-char hasn't been used any
   longer.
   If we're tabifying, all tabs have been converted to spaces by
   process_char().  Keep a count of consecutive spaces, and when
   a nonspace is encountered, call print_white_space() to print the
   required number of tabs and spaces. */

static void
print_char (char c)
{
  if (tabify_output)
    {
      if (c == ' ')
        {
          ++spaces_not_printed;
          return;
        }
      else if (spaces_not_printed > 0)
        print_white_space ();

      /* Nonprintables are assumed to have width 0, except '\b'. */
      if (! isprint (to_uchar (c)))
        {
          if (c == '\b')
            --output_position;
        }
      else
        ++output_position;
    }
  putchar (c);
}

