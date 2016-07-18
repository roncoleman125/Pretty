/* For debug mode, "underline" a key at the
   specified offset and screen width.  */

static void
mark_key (size_t offset, size_t width)
{
  while (offset--)
    putchar (' ');

  if (!width)
    printf (_("^ no match for key\n"));
  else
    {
      do
        putchar ('_');
      while (--width);

      putchar ('\n');
    }
}

