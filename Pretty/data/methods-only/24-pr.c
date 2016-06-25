/* Read a line with skip_to_page.

   Read from the current column's file until an end of line is
   hit.  Used when we read full lines to skip pages.
   With skip_to_page we have to check for FF-coincidence which is done
   in function read_line otherwise.
   Count lines of skipped pages to find the line number of 1st page
   printed relative to 1st line of input file (start_line_num). */

static void
skip_read (COLUMN *p, int column_number)
{
  int c;
  FILE *f = p->fp;
  int i;
  bool single_ff = false;
  COLUMN *q;

  /* Read 1st character in a line or any character succeeding a FF */
  if ((c = getc (f)) == '\f' && p->full_page_printed)
    /* A FF-coincidence with a previous full_page_printed.
       To avoid an additional empty page, eliminate the FF */
    if ((c = getc (f)) == '\n')
      c = getc (f);

  p->full_page_printed = false;

  /* 1st character a FF means a single FF without any printable
     characters. Don't count it as a line with -n option. */
  if (c == '\f')
    single_ff = true;

  /* Preparing for a FF-coincidence: Maybe we finish that page
     without a FF found */
  if (last_line)
    p->full_page_printed = true;

  while (c != '\n')
    {
      if (c == '\f')
        {
          /* No FF-coincidence possible,
             no catching up of a FF-coincidence with next page */
          if (last_line)
            {
              if (!parallel_files)
                for (q = column_vector, i = columns; i; ++q, --i)
                  q->full_page_printed = false;
              else
                p->full_page_printed = false;
            }

          if ((c = getc (f)) != '\n')
            ungetc (c, f);
          hold_file (p);
          break;
        }
      else if (c == EOF)
        {
          close_file (p);
          break;
        }
      c = getc (f);
    }

  if (skip_count)
    if ((!parallel_files || column_number == 1) && !single_ff)
      ++line_count;
}

