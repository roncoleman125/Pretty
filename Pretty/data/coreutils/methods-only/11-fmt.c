/* Set extra fields in word W describing any attached punctuation.  */

static void
check_punctuation (WORD *w)
{
  char const *start = w->text;
  char const *finish = start + (w->length - 1);
  unsigned char fin = *finish;

  w->paren = isopen (*start);
  w->punct = !! ispunct (fin);
  while (start < finish && isclose (*finish))
    finish--;
  w->period = isperiod (*finish);
}

