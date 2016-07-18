/* Close the file in P.

   If we aren't dealing with multiple files in parallel, we change
   the status of all columns in the column list to reflect the close. */

static void
close_file (COLUMN *p)
{
  COLUMN *q;
  int i;

  if (p->status == CLOSED)
    return;
  if (ferror (p->fp))
    error (EXIT_FAILURE, errno, "%s", p->name);
  if (fileno (p->fp) != STDIN_FILENO && fclose (p->fp) != 0)
    error (EXIT_FAILURE, errno, "%s", p->name);

  if (!parallel_files)
    {
      for (q = column_vector, i = columns; i; ++q, --i)
        {
          q->status = CLOSED;
          if (q->lines_stored == 0)
            {
              q->lines_to_print = 0;
            }
        }
    }
  else
    {
      p->status = CLOSED;
      p->lines_to_print = 0;
    }

  --files_ready_to_read;
}

