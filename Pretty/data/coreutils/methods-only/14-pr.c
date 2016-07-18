/* Set things up for printing a page

   Scan through the columns ...
   Determine which are ready to print
   (i.e., which have lines stored or open files)
   Set p->lines_to_print appropriately
   (to p->lines_stored if we're storing, or lines_per_body
   if we're reading straight from the file)
   Keep track of this total so we know when to stop printing */

static void
init_page (void)
{
  int j;
  COLUMN *p;

  if (storing_columns)
    {
      store_columns ();
      for (j = columns - 1, p = column_vector; j; --j, ++p)
        {
          p->lines_to_print = p->lines_stored;
        }

      /* Last column. */
      if (balance_columns)
        {
          p->lines_to_print = p->lines_stored;
        }
      /* Since we're not balancing columns, we don't need to store
         the rightmost column.   Read it straight from the file. */
      else
        {
          if (p->status == OPEN)
            {
              p->lines_to_print = lines_per_body;
            }
          else
            p->lines_to_print = 0;
        }
    }
  else
    for (j = columns, p = column_vector; j; --j, ++p)
      if (p->status == OPEN)
        {
          p->lines_to_print = lines_per_body;
        }
      else
        p->lines_to_print = 0;
}

