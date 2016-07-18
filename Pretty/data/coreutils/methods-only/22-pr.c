/* Pad to the bottom of the page.

   If the user has requested a formfeed, use one.
   Otherwise, use newlines. */

static void
pad_down (int lines)
{
  int i;

  if (use_form_feed)
    putchar ('\f');
  else
    for (i = lines; i; --i)
      putchar ('\n');
}

