/* Assuming cursor is at position FROM, indent up to position TO.
   Use a TAB character instead of two or more spaces whenever possible.  */

static void
indent (size_t from, size_t to)
{
  while (from < to)
    {
      if (tabsize != 0 && to / tabsize > (from + 1) / tabsize)
        {
          putchar ('\t');
          from += tabsize - from % tabsize;
        }
      else
        {
          putchar (' ');
          from++;
        }
    }
}

