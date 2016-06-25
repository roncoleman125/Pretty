/* Given a linebuffer LINE,
   return a pointer to the beginning of the line's field to be compared. */

static char *
find_field (struct linebuffer const *line)
{
  size_t count;
  char const *lp = line->buffer;
  size_t size = line->length - 1;
  size_t i = 0;

  for (count = 0; count < skip_fields && i < size; count++)
    {
      while (i < size && isblank (to_uchar (lp[i])))
        i++;
      while (i < size && !isblank (to_uchar (lp[i])))
        i++;
    }

  i += MIN (skip_chars, size - i);

  return line->buffer + i;
}

