/*-------------------------------------------------------------------.
| Output the current output fields as one line for a dumb terminal.  |
`-------------------------------------------------------------------*/

static void
output_one_dumb_line (void)
{
  if (!right_reference)
    {
      if (auto_reference)
        {

          /* Output the `reference' field, in such a way that GNU emacs
             next-error will handle it.  The ending colon is taken from the
             gap which follows.  */

          print_field (reference);
          putchar (':');
          print_spaces (reference_max_width
                        + gap_size
                        - (reference.end - reference.start)
                        - 1);
        }
      else
        {

          /* Output the `reference' field and its following gap.  */

          print_field (reference);
          print_spaces (reference_max_width
                        + gap_size
                        - (reference.end - reference.start));
        }
    }

  if (tail.start < tail.end)
    {
      /* Output the `tail' field.  */

      print_field (tail);
      if (tail_truncation)
        fputs (truncation_string, stdout);

      print_spaces (half_line_width - gap_size
                    - (before.end - before.start)
                    - (before_truncation ? truncation_string_length : 0)
                    - (tail.end - tail.start)
                    - (tail_truncation ? truncation_string_length : 0));
    }
  else
    print_spaces (half_line_width - gap_size
                  - (before.end - before.start)
                  - (before_truncation ? truncation_string_length : 0));

  /* Output the `before' field.  */

  if (before_truncation)
    fputs (truncation_string, stdout);
  print_field (before);

  print_spaces (gap_size);

  /* Output the `keyafter' field.  */

  print_field (keyafter);
  if (keyafter_truncation)
    fputs (truncation_string, stdout);

  if (head.start < head.end)
    {
      /* Output the `head' field.  */

      print_spaces (half_line_width
                    - (keyafter.end - keyafter.start)
                    - (keyafter_truncation ? truncation_string_length : 0)
                    - (head.end - head.start)
                    - (head_truncation ? truncation_string_length : 0));
      if (head_truncation)
        fputs (truncation_string, stdout);
      print_field (head);
    }
  else

    if ((auto_reference || input_reference) && right_reference)
      print_spaces (half_line_width
                    - (keyafter.end - keyafter.start)
                    - (keyafter_truncation ? truncation_string_length : 0));

  if ((auto_reference || input_reference) && right_reference)
    {
      /* Output the `reference' field.  */

      print_spaces (gap_size);
      print_field (reference);
    }

  putchar ('\n');
}

