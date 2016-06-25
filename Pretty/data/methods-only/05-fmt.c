/* Read a paragraph from input file F.  A paragraph consists of a
   maximal number of non-blank (excluding any prefix) lines subject to:
   * In split mode, a paragraph is a single non-blank line.
   * In crown mode, the second and subsequent lines must have the
   same indentation, but possibly different from the indent of the
   first line.
   * Tagged mode is similar, but the first and second lines must have
   different indentations.
   * Otherwise, all lines of a paragraph must have the same indent.
   If a prefix is in effect, it must be present at the same indent for
   each line in the paragraph.

   Return false if end-of-file was encountered before the start of a
   paragraph, else true.  */

static bool
get_paragraph (FILE *f)
{
  int c;

  last_line_length = 0;
  c = next_char;

  /* Scan (and copy) blank lines, and lines not introduced by the prefix.  */

  while (c == '\n' || c == EOF
         || next_prefix_indent < prefix_lead_space
         || in_column < next_prefix_indent + prefix_full_length)
    {
      c = copy_rest (f, c);
      if (c == EOF)
        {
          next_char = EOF;
          return false;
        }
      putchar ('\n');
      c = get_prefix (f);
    }

  /* Got a suitable first line for a paragraph.  */

  prefix_indent = next_prefix_indent;
  first_indent = in_column;
  wptr = parabuf;
  word_limit = word;
  c = get_line (f, c);
  set_other_indent (same_para (c));

  /* Read rest of paragraph (unless split is specified).  */

  if (split)
    {
      /* empty */
    }
  else if (crown)
    {
      if (same_para (c))
        {
          do
            {			/* for each line till the end of the para */
              c = get_line (f, c);
            }
          while (same_para (c) && in_column == other_indent);
        }
    }
  else if (tagged)
    {
      if (same_para (c) && in_column != first_indent)
        {
          do
            {			/* for each line till the end of the para */
              c = get_line (f, c);
            }
          while (same_para (c) && in_column == other_indent);
        }
    }
  else
    {
      while (same_para (c) && in_column == other_indent)
        c = get_line (f, c);
    }
  (word_limit - 1)->period = (word_limit - 1)->final = true;
  next_char = c;
  return true;
}

