/* Print (or store) padding until the current horizontal position
   is position. */

static void
pad_across_to (int position)
{
  int h = output_position;

  if (tabify_output)
    spaces_not_printed = position - output_position;
  else
    {
      while (++h <= position)
        putchar (' ');
      output_position = position;
    }
}

