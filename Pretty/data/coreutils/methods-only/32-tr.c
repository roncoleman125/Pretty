/* Read buffers of SIZE bytes from stdin until one is found that
   contains at least one character not in the delete set.  Store
   in the array BUF, all characters from that buffer that are not
   in the delete set, and return the number of characters saved
   or 0 upon EOF.  */

static size_t
read_and_delete (char *buf, size_t size)
{
  size_t n_saved;

  /* This enclosing do-while loop is to make sure that
     we don't return zero (indicating EOF) when we've
     just deleted all the characters in a buffer.  */
  do
    {
      size_t i;
      size_t nr = plain_read (buf, size);

      if (nr == 0)
        return 0;

      /* This first loop may be a waste of code, but gives much
         better performance when no characters are deleted in
         the beginning of a buffer.  It just avoids the copying
         of buf[i] into buf[n_saved] when it would be a NOP.  */

      for (i = 0; i < nr && !in_delete_set[to_uchar (buf[i])]; i++)
        continue;
      n_saved = i;

      for (++i; i < nr; i++)
        if (!in_delete_set[to_uchar (buf[i])])
          buf[n_saved++] = buf[i];
    }
  while (n_saved == 0);

  return n_saved;
}

