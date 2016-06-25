/* Read the rest of the line.

   Read from the current column's file until an end of line is
   hit.  Used when we've truncated a line and we no longer need
   to print or store its characters. */

static void
read_rest_of_line (COLUMN *p)
{
  int c;
  FILE *f = p->fp;

  while ((c = getc (f)) != '\n')
    {
      if (c == '\f')
        {
          if ((c = getc (f)) != '\n')
            ungetc (c, f);
          if (keep_FF)
            print_a_FF = true;
          hold_file (p);
          break;
        }
      else if (c == EOF)
        {
          close_file (p);
          break;
        }
    }
}

