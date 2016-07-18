/* Output to stdout the word W.  */

static void
put_word (WORD *w)
{
  const char *s;
  int n;

  s = w->text;
  for (n = w->length; n != 0; n--)
    putchar (*s++);
  out_column += w->length;
}

