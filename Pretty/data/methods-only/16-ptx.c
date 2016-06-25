/*---------------------------------------------------------.
| Output the current output fields as one line for `TeX'.  |
`---------------------------------------------------------*/

static void
output_one_tex_line (void)
{
  BLOCK key;			/* key field, isolated */
  BLOCK after;			/* after field, isolated */
  char *cursor;			/* running cursor in source text */

  printf ("\\%s ", macro_name);
  putchar ('{');
  print_field (tail);
  fputs ("}{", stdout);
  print_field (before);
  fputs ("}{", stdout);
  key.start = keyafter.start;
  after.end = keyafter.end;
  cursor = keyafter.start;
  SKIP_SOMETHING (cursor, keyafter.end);
  key.end = cursor;
  after.start = cursor;
  print_field (key);
  fputs ("}{", stdout);
  print_field (after);
  fputs ("}{", stdout);
  print_field (head);
  putchar ('}');
  if (auto_reference || input_reference)
    {
      putchar ('{');
      print_field (reference);
      putchar ('}');
    }
  putchar ('\n');
}

