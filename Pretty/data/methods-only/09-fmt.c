/* Read a prefix from input file F.  Return either first non-matching
   character, or first non-blank character after the prefix.  */

static int
get_prefix (FILE *f)
{
  int c;

  in_column = 0;
  c = get_space (f, getc (f));
  if (prefix_length == 0)
    next_prefix_indent = prefix_lead_space < in_column ?
      prefix_lead_space : in_column;
  else
    {
      const char *p;
      next_prefix_indent = in_column;
      for (p = prefix; *p != '\0'; p++)
        {
          unsigned char pc = *p;
          if (c != pc)
            return c;
          in_column++;
          c = getc (f);
        }
      c = get_space (f, c);
    }
  return c;
}

