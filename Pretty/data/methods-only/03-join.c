static void
freeline (struct line *line)
{
  if (line == NULL)
    return;
  free (line->fields);
  line->fields = NULL;
  free (line->buf.buffer);
  line->buf.buffer = NULL;
}

