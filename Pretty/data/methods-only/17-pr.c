/* Allocate space for storing columns.

   This is necessary when printing multiple columns from a single file.
   Lines are stored consecutively in buff, separated by '\0'.

   The following doesn't apply any longer - any tuning possible?
   (We can't use a fixed offset since with the '-s' flag lines aren't
   truncated.)

   We maintain a list (line_vector) of pointers to the beginnings
   of lines in buff.  We allocate one more than the number of lines
   because the last entry tells us the index of the last character,
   which we need to know in order to print the last line in buff. */

static void
init_store_cols (void)
{
  int total_lines = lines_per_body * columns;
  int chars_if_truncate = total_lines * (chars_per_column + 1);

  free (line_vector);
  /* FIXME: here's where it was allocated.  */
  line_vector = xmalloc ((total_lines + 1) * sizeof *line_vector);

  free (end_vector);
  end_vector = xmalloc (total_lines * sizeof *end_vector);

  free (buff);
  buff_allocated = (use_col_separator
                    ? 2 * chars_if_truncate
                    : chars_if_truncate);	/* Tune this. */
  buff = xmalloc (buff_allocated);
}

