/* Read a line from FP into LINE and split it into fields.
   Return true if successful.  */

static bool
get_line (FILE *fp, struct line **linep, int which)
{
  struct line *line = *linep;

  if (line == prevline[which - 1])
    {
      SWAPLINES (line, spareline[which - 1]);
      *linep = line;
    }

  if (line)
    reset_line (line);
  else
    line = init_linep (linep);

  if (! readlinebuffer (&line->buf, fp))
    {
      if (ferror (fp))
        error (EXIT_FAILURE, errno, _("read error"));
      freeline (line);
      return false;
    }

  xfields (line);

  if (prevline[which - 1])
    check_order (prevline[which - 1], line, which);

  prevline[which - 1] = line;
  return true;
}

