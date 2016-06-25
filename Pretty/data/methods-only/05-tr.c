/* Perform the first pass over each range-spec argument S, converting all
   \c and \ddd escapes to their one-byte representations.  If an invalid
   quote sequence is found print an error message and return false;
   Otherwise set *ES to the resulting string and return true.
   The resulting array of characters may contain zero-bytes;
   however, on input, S is assumed to be null-terminated, and hence
   cannot contain actual (non-escaped) zero bytes.  */

static bool
unquote (char const *s, struct E_string *es)
{
  size_t i, j;
  size_t len = strlen (s);

  es->s = xmalloc (len);
  es->escaped = xcalloc (len, sizeof es->escaped[0]);

  j = 0;
  for (i = 0; s[i]; i++)
    {
      unsigned char c;
      int oct_digit;

      switch (s[i])
        {
        case '\\':
          es->escaped[j] = true;
          switch (s[i + 1])
            {
            case '\\':
              c = '\\';
              break;
            case 'a':
              c = '\a';
              break;
            case 'b':
              c = '\b';
              break;
            case 'f':
              c = '\f';
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'v':
              c = '\v';
              break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
              c = s[i + 1] - '0';
              oct_digit = s[i + 2] - '0';
              if (0 <= oct_digit && oct_digit <= 7)
                {
                  c = 8 * c + oct_digit;
                  ++i;
                  oct_digit = s[i + 2] - '0';
                  if (0 <= oct_digit && oct_digit <= 7)
                    {
                      if (8 * c + oct_digit < N_CHARS)
                        {
                          c = 8 * c + oct_digit;
                          ++i;
                        }
                      else
                        {
                          /* A 3-digit octal number larger than \377 won't
                             fit in 8 bits.  So we stop when adding the
                             next digit would put us over the limit and
                             give a warning about the ambiguity.  POSIX
                             isn't clear on this, and we interpret this
                             lack of clarity as meaning the resulting behavior
                             is undefined, which means we're allowed to issue
                             a warning.  */
                          error (0, 0, _("warning: the ambiguous octal escape \
\\%c%c%c is being\n\tinterpreted as the 2-byte sequence \\0%c%c, %c"),
                                 s[i], s[i + 1], s[i + 2],
                                 s[i], s[i + 1], s[i + 2]);
                        }
                    }
                }
              break;
            case '\0':
              error (0, 0, _("warning: an unescaped backslash "
                             "at end of string is not portable"));
              /* POSIX is not clear about this.  */
              es->escaped[j] = false;
              i--;
              c = '\\';
              break;
            default:
              c = s[i + 1];
              break;
            }
          ++i;
          es->s[j++] = c;
          break;
        default:
          es->s[j++] = s[i];
          break;
        }
    }
  es->len = j;
  return true;
}

