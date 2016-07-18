/* Fill in the `fields' structure in LINE.  */

static void
xfields (struct line *line)
{
  char *ptr = line->buf.buffer;
  char const *lim = ptr + line->buf.length - 1;

  if (ptr == lim)
    return;

  if (0 <= tab && tab != '\n')
    {
      char *sep;
      for (; (sep = memchr (ptr, tab, lim - ptr)) != NULL; ptr = sep + 1)
        extract_field (line, ptr, sep - ptr);
    }
  else if (tab < 0)
    {
      /* Skip leading blanks before the first field.  */
      while (isblank (to_uchar (*ptr)))
        if (++ptr == lim)
          return;

      do
        {
          char *sep;
          for (sep = ptr + 1; sep != lim && ! isblank (to_uchar (*sep)); sep++)
            continue;
          extract_field (line, ptr, sep - ptr);
          if (sep == lim)
            return;
          for (ptr = sep + 1; ptr != lim && isblank (to_uchar (*ptr)); ptr++)
            continue;
        }
      while (ptr != lim);
    }

  extract_field (line, ptr, lim - ptr);
}

