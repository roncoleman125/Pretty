/* Return the number of columns that have either an open file or
   stored lines. */

static int
cols_ready_to_print (void)
{
  COLUMN *q;
  int i;
  int n;

  n = 0;
  for (q = column_vector, i = 0; i < columns; ++q, ++i)
    if (q->status == OPEN ||
        q->status == FF_FOUND ||	/* With -b: To print a header only */
        (storing_columns && q->lines_stored > 0 && q->lines_to_print > 0))
      ++n;
  return n;
}

