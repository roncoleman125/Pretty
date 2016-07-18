/* Read data from file IN.  Input lines are delimited by EOLBYTE;
   silently append a trailing EOLBYTE if the file ends in some other
   byte.  Store a pointer to the resulting array of lines into *PLINE.
   Return the number of lines read.  Report an error and exit on
   failure.  */

static size_t
read_input (FILE *in, char eolbyte, char ***pline)
{
  char *p;
  char *buf = NULL;
  size_t used;
  char *lim;
  char **line;
  size_t i;
  size_t n_lines;

  if (!(buf = fread_file (in, &used)))
    error (EXIT_FAILURE, errno, _("read error"));

  if (used && buf[used - 1] != eolbyte)
    buf[used++] = eolbyte;

  lim = buf + used;

  n_lines = 0;
  for (p = buf; p < lim; p = next_line (p, eolbyte, lim - p))
    n_lines++;

  *pline = line = xnmalloc (n_lines + 1, sizeof *line);

  line[0] = p = buf;
  for (i = 1; i <= n_lines; i++)
    line[i] = p = next_line (p, eolbyte, lim - p);

  return n_lines;
}

