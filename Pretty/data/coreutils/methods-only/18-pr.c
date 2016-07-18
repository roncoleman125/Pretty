/* Store all but the rightmost column.
   (Used when printing a single file in multiple downward columns)

   For each column
   set p->current_line to be the index in line_vector of the
   first line in the column
   For each line in the column
   store the line in buff
   add to line_vector the index of the line's first char
   buff_start is the index in buff of the first character in the
   current line. */

static void
store_columns (void)
{
  int i, j;
  unsigned int line = 0;
  unsigned int buff_start;
  int last_col;		/* The rightmost column which will be saved in buff */
  COLUMN *p;

  buff_current = 0;
  buff_start = 0;

  if (balance_columns)
    last_col = columns;
  else
    last_col = columns - 1;

  for (i = 1, p = column_vector; i <= last_col; ++i, ++p)
    p->lines_stored = 0;

  for (i = 1, p = column_vector; i <= last_col && files_ready_to_read;
       ++i, ++p)
    {
      p->current_line = line;
      for (j = lines_per_body; j && files_ready_to_read; --j)

        if (p->status == OPEN)	/* Redundant.  Clean up. */
          {
            input_position = 0;

            if (!read_line (p))
              read_rest_of_line (p);

            if (p->status == OPEN
                || buff_start != buff_current)
              {
                ++p->lines_stored;
                line_vector[line] = buff_start;
                end_vector[line++] = input_position;
                buff_start = buff_current;
              }
          }
    }

  /* Keep track of the location of the last char in buff. */
  line_vector[line] = buff_start;

  if (balance_columns)
    balance (line);
}

static void
balance (int total_stored)
{
  COLUMN *p;
  int i, lines;
  int first_line = 0;

  for (i = 1, p = column_vector; i <= columns; ++i, ++p)
    {
      lines = total_stored / columns;
      if (i <= total_stored % columns)
        ++lines;

      p->lines_stored = lines;
      p->current_line = first_line;

      first_line += lines;
    }
}

