/* We've just printed some files and need to clean up things before
   looking for more options and printing the next batch of files.

   Free everything we've xmalloc'ed, except `header'. */

static void
cleanup (void)
{
  free (number_buff);
  free (clump_buff);
  free (column_vector);
  free (line_vector);
  free (end_vector);
  free (buff);
}

