/* Store a character in the buffer. */

static void
store_char (char c)
{
  if (buff_current >= buff_allocated)
    {
      /* May be too generous. */
      buff = X2REALLOC (buff, &buff_allocated);
    }
  buff[buff_current++] = c;
}

