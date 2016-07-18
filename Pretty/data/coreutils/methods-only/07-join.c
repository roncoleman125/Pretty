static struct line *
init_linep (struct line **linep)
{
  struct line *line = xmalloc (sizeof *line);
  memset (line, '\0', sizeof *line);
  *linep = line;
  return line;
}

