/* Record a field in LINE, with location FIELD and size LEN.  */

static void
extract_field (struct line *line, char *field, size_t len)
{
  if (line->nfields >= line->nfields_allocated)
    {
      line->fields = X2NREALLOC (line->fields, &line->nfields_allocated);
    }
  line->fields[line->nfields].beg = field;
  line->fields[line->nfields].len = len;
  ++(line->nfields);
}

