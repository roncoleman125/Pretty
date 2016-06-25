/* Allocate enough column info suitable for the current number of
   files and display columns, and initialize the info to represent the
   narrowest possible columns.  */

static void
init_column_info (void)
{
  size_t i;
  size_t max_cols = MIN (max_idx, cwd_n_used);

  /* Currently allocated columns in column_info.  */
  static size_t column_info_alloc;

  if (column_info_alloc < max_cols)
    {
      size_t new_column_info_alloc;
      size_t *p;

      if (max_cols < max_idx / 2)
        {
          /* The number of columns is far less than the display width
             allows.  Grow the allocation, but only so that it's
             double the current requirements.  If the display is
             extremely wide, this avoids allocating a lot of memory
             that is never needed.  */
          column_info = xnrealloc (column_info, max_cols,
                                   2 * sizeof *column_info);
          new_column_info_alloc = 2 * max_cols;
        }
      else
        {
          column_info = xnrealloc (column_info, max_idx, sizeof *column_info);
          new_column_info_alloc = max_idx;
        }

      /* Allocate the new size_t objects by computing the triangle
         formula n * (n + 1) / 2, except that we don't need to
         allocate the part of the triangle that we've already
         allocated.  Check for address arithmetic overflow.  */
      {
        size_t column_info_growth = new_column_info_alloc - column_info_alloc;
        size_t s = column_info_alloc + 1 + new_column_info_alloc;
        size_t t = s * column_info_growth;
        if (s < new_column_info_alloc || t / column_info_growth != s)
          xalloc_die ();
        p = xnmalloc (t / 2, sizeof *p);
      }

      /* Grow the triangle by parceling out the cells just allocated.  */
      for (i = column_info_alloc; i < new_column_info_alloc; i++)
        {
          column_info[i].col_arr = p;
          p += i + 1;
        }

      column_info_alloc = new_column_info_alloc;
    }

  for (i = 0; i < max_cols; ++i)
    {
      size_t j;

      column_info[i].valid_len = true;
      column_info[i].line_len = (i + 1) * MIN_COLUMN_WIDTH;
      for (j = 0; j <= i; ++j)
        column_info[i].col_arr[j] = MIN_COLUMN_WIDTH;
    }
}

