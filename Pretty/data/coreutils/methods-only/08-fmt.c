/* Read a line from input file F, given first non-blank character C
   after the prefix, and the following indent, and break it into words.
   A word is a maximal non-empty string of non-white characters.  A word
   ending in [.?!]["')\]]* and followed by end-of-line or at least two
   spaces ends a sentence, as in emacs.

   Return the first non-blank character of the next line.  */

static int
get_line (FILE *f, int c)
{
  int start;
  char *end_of_parabuf;
  WORD *end_of_word;

  end_of_parabuf = &parabuf[MAXCHARS];
  end_of_word = &word[MAXWORDS - 2];

  do
    {				/* for each word in a line */

      /* Scan word.  */

      word_limit->text = wptr;
      do
        {
          if (wptr == end_of_parabuf)
            {
              set_other_indent (true);
              flush_paragraph ();
            }
          *wptr++ = c;
          c = getc (f);
        }
      while (c != EOF && !isspace (c));
      in_column += word_limit->length = wptr - word_limit->text;
      check_punctuation (word_limit);

      /* Scan inter-word space.  */

      start = in_column;
      c = get_space (f, c);
      word_limit->space = in_column - start;
      word_limit->final = (c == EOF
                           || (word_limit->period
                               && (c == '\n' || word_limit->space > 1)));
      if (c == '\n' || c == EOF || uniform)
        word_limit->space = word_limit->final ? 2 : 1;
      if (word_limit == end_of_word)
        {
          set_other_indent (true);
          flush_paragraph ();
        }
      word_limit++;
    }
  while (c != '\n' && c != EOF);
  return get_prefix (f);
}

