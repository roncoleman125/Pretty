/* Output to stdout a paragraph from word up to (but not including)
   FINISH, which must be in the next_break chain from word.  */

static void
put_paragraph (WORD *finish)
{
  WORD *w;

  put_line (word, first_indent);
  for (w = word->next_break; w != finish; w = w->next_break)
    put_line (w, other_indent);
}

