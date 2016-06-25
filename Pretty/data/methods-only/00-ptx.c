/*------------------------------------------------------.
| Duplicate string STRING, while evaluating \-escapes.  |
`------------------------------------------------------*/

/* Loosely adapted from GNU sh-utils printf.c code.  */

static char *
copy_unescaped_string (const char *string)
{
  char *result;			/* allocated result */
  char *cursor;			/* cursor in result */
  int value;			/* value of \nnn escape */
  int length;			/* length of \nnn escape */

  result = xmalloc (strlen (string) + 1);
  cursor = result;

  while (*string)
    {
      if (*string == '\\')
        {
          string++;
          switch (*string)
            {
            case 'x':		/* \xhhh escape, 3 chars maximum */
              value = 0;
              for (length = 0, string++;
                   length < 3 && isxdigit (to_uchar (*string));
                   length++, string++)
                value = value * 16 + HEXTOBIN (*string);
              if (length == 0)
                {
                  *cursor++ = '\\';
                  *cursor++ = 'x';
                }
              else
                *cursor++ = value;
              break;

            case '0':		/* \0ooo escape, 3 chars maximum */
              value = 0;
              for (length = 0, string++;
                   length < 3 && ISODIGIT (*string);
                   length++, string++)
                value = value * 8 + OCTTOBIN (*string);
              *cursor++ = value;
              break;

            case 'a':		/* alert */
              *cursor++ = '\a';
              *cursor++ = 7;
              string++;
              break;

            case 'b':		/* backspace */
              *cursor++ = '\b';
              string++;
              break;

            case 'c':		/* cancel the rest of the output */
              while (*string)
                string++;
              break;

            case 'f':		/* form feed */
              *cursor++ = '\f';
              string++;
              break;

            case 'n':		/* new line */
              *cursor++ = '\n';
              string++;
              break;

            case 'r':		/* carriage return */
              *cursor++ = '\r';
              string++;
              break;

            case 't':		/* horizontal tab */
              *cursor++ = '\t';
              string++;
              break;

            case 'v':		/* vertical tab */
              *cursor++ = '\v';
              *cursor++ = 11;
              string++;
              break;

            case '\0':		/* lone backslash at end of string */
              /* ignore it */
              break;

            default:
              *cursor++ = '\\';
              *cursor++ = *string++;
              break;
            }
        }
      else
        *cursor++ = *string++;
    }

  *cursor = '\0';
  return result;
}

