/* Parse a string as part of the LS_COLORS variable; this may involve
   decoding all kinds of escape characters.  If equals_end is set an
   unescaped equal sign ends the string, otherwise only a : or \0
   does.  Set *OUTPUT_COUNT to the number of bytes output.  Return
   true if successful.

   The resulting string is *not* null-terminated, but may contain
   embedded nulls.

   Note that both dest and src are char **; on return they point to
   the first free byte after the array and the character that ended
   the input string, respectively.  */

static bool
get_funky_string (char **dest, const char **src, bool equals_end,
                  size_t *output_count)
{
  char num;			/* For numerical codes */
  size_t count;			/* Something to count with */
  enum {
    ST_GND, ST_BACKSLASH, ST_OCTAL, ST_HEX, ST_CARET, ST_END, ST_ERROR
  } state;
  const char *p;
  char *q;

  p = *src;			/* We don't want to double-indirect */
  q = *dest;			/* the whole darn time.  */

  count = 0;			/* No characters counted in yet.  */
  num = 0;

  state = ST_GND;		/* Start in ground state.  */
  while (state < ST_END)
    {
      switch (state)
        {
        case ST_GND:		/* Ground state (no escapes) */
          switch (*p)
            {
            case ':':
            case '\0':
              state = ST_END;	/* End of string */
              break;
            case '\\':
              state = ST_BACKSLASH; /* Backslash scape sequence */
              ++p;
              break;
            case '^':
              state = ST_CARET; /* Caret escape */
              ++p;
              break;
            case '=':
              if (equals_end)
                {
                  state = ST_END; /* End */
                  break;
                }
              /* else fall through */
            default:
              *(q++) = *(p++);
              ++count;
              break;
            }
          break;

        case ST_BACKSLASH:	/* Backslash escaped character */
          switch (*p)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
              state = ST_OCTAL;	/* Octal sequence */
              num = *p - '0';
              break;
            case 'x':
            case 'X':
              state = ST_HEX;	/* Hex sequence */
              num = 0;
              break;
            case 'a':		/* Bell */
              num = '\a';
              break;
            case 'b':		/* Backspace */
              num = '\b';
              break;
            case 'e':		/* Escape */
              num = 27;
              break;
            case 'f':		/* Form feed */
              num = '\f';
              break;
            case 'n':		/* Newline */
              num = '\n';
              break;
            case 'r':		/* Carriage return */
              num = '\r';
              break;
            case 't':		/* Tab */
              num = '\t';
              break;
            case 'v':		/* Vtab */
              num = '\v';
              break;
            case '?':		/* Delete */
              num = 127;
              break;
            case '_':		/* Space */
              num = ' ';
              break;
            case '\0':		/* End of string */
              state = ST_ERROR;	/* Error! */
              break;
            default:		/* Escaped character like \ ^ : = */
              num = *p;
              break;
            }
          if (state == ST_BACKSLASH)
            {
              *(q++) = num;
              ++count;
              state = ST_GND;
            }
          ++p;
          break;

        case ST_OCTAL:		/* Octal sequence */
          if (*p < '0' || *p > '7')
            {
              *(q++) = num;
              ++count;
              state = ST_GND;
            }
          else
            num = (num << 3) + (*(p++) - '0');
          break;

        case ST_HEX:		/* Hex sequence */
          switch (*p)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
              num = (num << 4) + (*(p++) - '0');
              break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
              num = (num << 4) + (*(p++) - 'a') + 10;
              break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
              num = (num << 4) + (*(p++) - 'A') + 10;
              break;
            default:
              *(q++) = num;
              ++count;
              state = ST_GND;
              break;
            }
          break;

        case ST_CARET:		/* Caret escape */
          state = ST_GND;	/* Should be the next state... */
          if (*p >= '@' && *p <= '~')
            {
              *(q++) = *(p++) & 037;
              ++count;
            }
          else if (*p == '?')
            {
              *(q++) = 127;
              ++count;
            }
          else
            state = ST_ERROR;
          break;

        default:
          abort ();
        }
    }

  *dest = q;
  *src = p;
  *output_count = count;

  return state != ST_ERROR;
}

