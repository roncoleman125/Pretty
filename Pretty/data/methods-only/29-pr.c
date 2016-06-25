/* Skip to page PAGE before printing.
   PAGE may be larger than total number of pages. */

static bool
skip_to_page (uintmax_t page)
{
  uintmax_t n;
  int i;
  int j;
  COLUMN *p;

  for (n = 1; n < page; ++n)
    {
      for (i = 1; i < lines_per_body; ++i)
        {
          for (j = 1, p = column_vector; j <= columns; ++j, ++p)
            if (p->status == OPEN)
              skip_read (p, j);
        }
      last_line = true;
      for (j = 1, p = column_vector; j <= columns; ++j, ++p)
        if (p->status == OPEN)
          skip_read (p, j);

      if (storing_columns)	/* change FF_FOUND to ON_HOLD */
        for (j = 1, p = column_vector; j <= columns; ++j, ++p)
          if (p->status != CLOSED)
            p->status = ON_HOLD;

      reset_status ();
      last_line = false;

      if (files_ready_to_read < 1)
        {
          /* It's very helpful, normally the total number of pages is
             not known in advance.  */
          error (0, 0,
                 _("starting page number %"PRIuMAX
                   " exceeds page count %"PRIuMAX),
                 page, n);
          break;
        }
    }
  return files_ready_to_read > 0;
}

