/* Open the necessary files,
   maintaining a COLUMN structure for each column.

   With multiple files, each column p has a different p->fp.
   With single files, each column p has the same p->fp.
   Return false if (number_of_files > 0) and no files can be opened,
   true otherwise.

   With each column/file p, p->full_page_printed is initialized,
   see also open_file.  */

static bool
init_fps (int number_of_files, char **av)
{
  int i, files_left;
  COLUMN *p;
  FILE *firstfp;
  char const *firstname;

  total_files = 0;

  free (column_vector);
  column_vector = xnmalloc (columns, sizeof (COLUMN));

  if (parallel_files)
    {
      files_left = number_of_files;
      for (p = column_vector; files_left--; ++p, ++av)
        {
          if (! open_file (*av, p))
            {
              --p;
              --columns;
            }
        }
      if (columns == 0)
        return false;
      init_header ("", -1);
    }
  else
    {
      p = column_vector;
      if (number_of_files > 0)
        {
          if (! open_file (*av, p))
            return false;
          init_header (*av, fileno (p->fp));
          p->lines_stored = 0;
        }
      else
        {
          p->name = _("standard input");
          p->fp = stdin;
          have_read_stdin = true;
          p->status = OPEN;
          p->full_page_printed = false;
          ++total_files;
          init_header ("", -1);
          p->lines_stored = 0;
        }

      firstname = p->name;
      firstfp = p->fp;
      for (i = columns - 1, ++p; i; --i, ++p)
        {
          p->name = firstname;
          p->fp = firstfp;
          p->status = OPEN;
          p->full_page_printed = false;
          p->lines_stored = 0;
        }
    }
  files_ready_to_read = total_files;
  return true;
}

