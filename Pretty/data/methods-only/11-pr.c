/* Undo hold_file -- go through the column list and change any
   ON_HOLD columns to OPEN.  Used at the end of each page. */

static void
reset_status (void)
{
  int i = columns;
  COLUMN *p;

  for (p = column_vector; i; --i, ++p)
    if (p->status == ON_HOLD)
      {
        p->status = OPEN;
        files_ready_to_read++;
      }

  if (storing_columns)
    {
      if (column_vector->status == CLOSED)
        /* We use the info to output an error message in  skip_to_page. */
        files_ready_to_read = 0;
      else
        files_ready_to_read = 1;
    }
}

