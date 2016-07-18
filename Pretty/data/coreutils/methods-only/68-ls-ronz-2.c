/* Output to OUT a quoted representation of the file name NAME,
   using OPTIONS to control quoting.  Produce no output if OUT is NULL.
   Store the number of screen columns occupied by NAME's quoted
   representation into WIDTH, if non-NULL.  Return the number of bytes
   produced.  */

static size_t
quote_name (FILE *out, const char *name, struct quoting_options const *options,
            size_t *width)
{
  char smallbuf[BUFSIZ];
  size_t len = quotearg_buffer (smallbuf, sizeof smallbuf, name, -1, options);
  char *buf;
  size_t displayed_width IF_LINT ( = 0);

  if (len < sizeof smallbuf)
    buf = smallbuf;
  else
    {
      buf = alloca (len + 1);
      quotearg_buffer (buf, len + 1, name, -1, options);
    }

  if (qmark_funny_chars)
    {
      if (MB_CUR_MAX > 1)
        {
          char const *p = buf;
          char const *plimit = buf + len;
          char *q = buf;
          displayed_width = 0;

          while (p < plimit)
            switch (*p)
              {
                case ' ': case '!': case '"': case '#': case '%':
                case '&': case '\'': case '(': case ')': case '*':
                case '+': case ',': case '-': case '.': case '/':
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case ':': case ';': case '<': case '=': case '>':
                case '?':
                case 'A': case 'B': case 'C': case 'D': case 'E':
                case 'F': case 'G': case 'H': case 'I': case 'J':
                case 'K': case 'L': case 'M': case 'N': case 'O':
                case 'P': case 'Q': case 'R': case 'S': case 'T':
                case 'U': case 'V': case 'W': case 'X': case 'Y':
                case 'Z':
                case '[': case '\\': case ']': case '^': case '_':
                case 'a': case 'b': case 'c': case 'd': case 'e':
                case 'f': case 'g': case 'h': case 'i': case 'j':
                case 'k': case 'l': case 'm': case 'n': case 'o':
                case 'p': case 'q': case 'r': case 's': case 't':
                case 'u': case 'v': case 'w': case 'x': case 'y':
                case 'z': case '{': case '|': case '}': case '~':
                  /* These characters are printable ASCII characters.  */
                  *q++ = *p++;
                  displayed_width += 1;
                  break;
                default:
                  /* If we have a multibyte sequence, copy it until we
                     reach its end, replacing each non-printable multibyte
                     character with a single question mark.  */
                  {
                    DECLARE_ZEROED_AGGREGATE (mbstate_t, mbstate);
                    do
                      {
                        wchar_t wc;
                        size_t bytes;
                        int w;

                        bytes = mbrtowc (&wc, p, plimit - p, &mbstate);

                        if (bytes == (size_t) -1)
                          {
                            /* An invalid multibyte sequence was
                               encountered.  Skip one input byte, and
                               put a question mark.  */
                            p++;
                            *q++ = '?';
                            displayed_width += 1;
                            break;
                          }

                        if (bytes == (size_t) -2)
                          {
                            /* An incomplete multibyte character
                               at the end.  Replace it entirely with
                               a question mark.  */
                            p = plimit;
                            *q++ = '?';
                            displayed_width += 1;
                            break;
                          }

                        if (bytes == 0)
                          /* A null wide character was encountered.  */
                          bytes = 1;

                        w = wcwidth (wc);
                        if (w >= 0)
                          {
                            /* A printable multibyte character.
                               Keep it.  */
                            for (; bytes > 0; --bytes)
                              *q++ = *p++;
                            displayed_width += w;
                          }
                        else
                          {
                            /* An unprintable multibyte character.
                               Replace it entirely with a question
                               mark.  */
                            p += bytes;
                            *q++ = '?';
                            displayed_width += 1;
                          }
                      }
                    while (! mbsinit (&mbstate));
                  }
                  break;
              }

          /* The buffer may have shrunk.  */
          len = q - buf;
        }
      else
        {
          char *p = buf;
          char const *plimit = buf + len;

          while (p < plimit)
            {
              if (! isprint (to_uchar (*p)))
                *p = '?';
              p++;
            }
          displayed_width = len;
        }
    }
  else if (width != NULL)
    {
      if (MB_CUR_MAX > 1)
        displayed_width = mbsnwidth (buf, len, 0);
      else
        {
          char const *p = buf;
          char const *plimit = buf + len;

          displayed_width = 0;
          while (p < plimit)
            {
              if (isprint (to_uchar (*p)))
                displayed_width++;
              p++;
            }
        }
    }

  if (out != NULL)
    fwrite (buf, 1, len, out);
  if (width != NULL)
    *width = displayed_width;
  return len;
}

