/* Read blank characters from input file F, starting with C, and keeping
   in_column up-to-date.  Return first non-blank character.  */

static int
get_space (FILE *f, int c)
{
  while (true)
    {
      if (c == ' ')
        in_column++;
      else if (c == '\t')
        {
          tabs = true;
          in_column = (in_column / TABWIDTH + 1) * TABWIDTH;
        }
      else
        return c;
      c = getc (f);
    }
}

