/* Assuming the current column is COLUMN, return the column that
   printing C will move the cursor to.
   The first column is 0. */

static size_t
adjust_column (size_t column, char c)
{
  if (!count_bytes)
    {
      if (c == '\b')
        {
          if (column > 0)
            column--;
        }
      else if (c == '\r')
        column = 0;
      else if (c == '\t')
        column += TAB_WIDTH - column % TAB_WIDTH;
      else /* if (isprint (c)) */
        column++;
    }
  else
    column++;
  return column;
}

