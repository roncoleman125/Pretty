/* Return a newly allocated copy of S which is suitable for printing.
   LEN is the number of characters in S.  Most non-printing
   (isprint) characters are represented by a backslash followed by
   3 octal digits.  However, the characters represented by \c escapes
   where c is one of [abfnrtv] are represented by their 2-character \c
   sequences.  This function is used solely for printing error messages.  */

static char *
make_printable_str (char const *s, size_t len)
{
  /* Worst case is that every character expands to a backslash
     followed by a 3-character octal escape sequence.  */
  char *printable_buf = xnmalloc (len + 1, 4);
  char *p = printable_buf;
  size_t i;

  for (i = 0; i < len; i++)
    {
      char buf[5];
      char const *tmp = NULL;
      unsigned char c = s[i];

      switch (c)
        {
        case '\\':
          tmp = "\\";
          break;
        case '\a':
          tmp = "\\a";
          break;
        case '\b':
          tmp = "\\b";
          break;
        case '\f':
          tmp = "\\f";
          break;
        case '\n':
          tmp = "\\n";
          break;
        case '\r':
          tmp = "\\r";
          break;
        case '\t':
          tmp = "\\t";
          break;
        case '\v':
          tmp = "\\v";
          break;
        default:
          if (isprint (c))
            {
              buf[0] = c;
              buf[1] = '\0';
            }
          else
            sprintf (buf, "\\%03o", c);
          tmp = buf;
          break;
        }
      p = stpcpy (p, tmp);
    }
  return printable_buf;
}

