/* Print field N of LINE if it exists and is nonempty, otherwise
   `empty_filler' if it is nonempty.  */

static void
prfield (size_t n, struct line const *line)
{
  size_t len;

  if (n < line->nfields)
    {
      len = line->fields[n].len;
      if (len)
        fwrite (line->fields[n].beg, 1, len, stdout);
      else if (empty_filler)
        fputs (empty_filler, stdout);
    }
  else if (empty_filler)
    fputs (empty_filler, stdout);
}

