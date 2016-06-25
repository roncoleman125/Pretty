/* read file F and send formatted output to stdout.  */

static void
fmt (FILE *f)
{
  fadvise (f, FADVISE_SEQUENTIAL);
  tabs = false;
  other_indent = 0;
  next_char = get_prefix (f);
  while (get_paragraph (f))
    {
      fmt_paragraph ();
      put_paragraph (word_limit);
    }
}

