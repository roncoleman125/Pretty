/* Formatting and actual output - control routines.  */

/*----------------------------------------------------------------------.
| Output the current output fields as one line for `troff' or `nroff'.  |
`----------------------------------------------------------------------*/

static void
output_one_roff_line (void)
{
  /* Output the `tail' field.  */

  printf (".%s \"", macro_name);
  print_field (tail);
  if (tail_truncation)
    fputs (truncation_string, stdout);
  putchar ('"');

  /* Output the `before' field.  */

  fputs (" \"", stdout);
  if (before_truncation)
    fputs (truncation_string, stdout);
  print_field (before);
  putchar ('"');

  /* Output the `keyafter' field.  */

  fputs (" \"", stdout);
  print_field (keyafter);
  if (keyafter_truncation)
    fputs (truncation_string, stdout);
  putchar ('"');

  /* Output the `head' field.  */

  fputs (" \"", stdout);
  if (head_truncation)
    fputs (truncation_string, stdout);
  print_field (head);
  putchar ('"');

  /* Conditionally output the `reference' field.  */

  if (auto_reference || input_reference)
    {
      fputs (" \"", stdout);
      print_field (reference);
      putchar ('"');
    }

  putchar ('\n');
}

