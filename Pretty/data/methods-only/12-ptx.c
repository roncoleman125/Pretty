/*-------------------------------------.
| Prints the field provided by FIELD.  |
`-------------------------------------*/

static void
print_field (BLOCK field)
{
  char *cursor;			/* Cursor in field to print */
  int base;			/* Base character, without diacritic */
  int diacritic;		/* Diacritic code for the character */

  /* Whitespace is not really compressed.  Instead, each white space
     character (tab, vt, ht etc.) is printed as one single space.  */

  for (cursor = field.start; cursor < field.end; cursor++)
    {
      unsigned char character = *cursor;
      if (edited_flag[character])
        {

          /* First check if this is a diacriticized character.

             This works only for TeX.  I do not know how diacriticized
             letters work with `roff'.  Please someone explain it to me!  */

          diacritic = todiac (character);
          if (diacritic != 0 && output_format == TEX_FORMAT)
            {
              base = tobase (character);
              switch (diacritic)
                {

                case 1:		/* Latin diphthongs */
                  switch (base)
                    {
                    case 'o':
                      fputs ("\\oe{}", stdout);
                      break;

                    case 'O':
                      fputs ("\\OE{}", stdout);
                      break;

                    case 'a':
                      fputs ("\\ae{}", stdout);
                      break;

                    case 'A':
                      fputs ("\\AE{}", stdout);
                      break;

                    default:
                      putchar (' ');
                    }
                  break;

                case 2:		/* Acute accent */
                  printf ("\\'%s%c", (base == 'i' ? "\\" : ""), base);
                  break;

                case 3:		/* Grave accent */
                  printf ("\\`%s%c", (base == 'i' ? "\\" : ""), base);
                  break;

                case 4:		/* Circumflex accent */
                  printf ("\\^%s%c", (base == 'i' ? "\\" : ""), base);
                  break;

                case 5:		/* Diaeresis */
                  printf ("\\\"%s%c", (base == 'i' ? "\\" : ""), base);
                  break;

                case 6:		/* Tilde accent */
                  printf ("\\~%s%c", (base == 'i' ? "\\" : ""), base);
                  break;

                case 7:		/* Cedilla */
                  printf ("\\c{%c}", base);
                  break;

                case 8:		/* Small circle beneath */
                  switch (base)
                    {
                    case 'a':
                      fputs ("\\aa{}", stdout);
                      break;

                    case 'A':
                      fputs ("\\AA{}", stdout);
                      break;

                    default:
                      putchar (' ');
                    }
                  break;

                case 9:		/* Strike through */
                  switch (base)
                    {
                    case 'o':
                      fputs ("\\o{}", stdout);
                      break;

                    case 'O':
                      fputs ("\\O{}", stdout);
                      break;

                    default:
                      putchar (' ');
                    }
                  break;
                }
            }
          else

            /* This is not a diacritic character, so handle cases which are
               really specific to `roff' or TeX.  All white space processing
               is done as the default case of this switch.  */

            switch (character)
              {
              case '"':
                /* In roff output format, double any quote.  */
                putchar ('"');
                putchar ('"');
                break;

              case '$':
              case '%':
              case '&':
              case '#':
              case '_':
                /* In TeX output format, precede these with a backslash.  */
                putchar ('\\');
                putchar (character);
                break;

              case '{':
              case '}':
                /* In TeX output format, precede these with a backslash and
                   force mathematical mode.  */
                printf ("$\\%c$", character);
                break;

              case '\\':
                /* In TeX output mode, request production of a backslash.  */
                fputs ("\\backslash{}", stdout);
                break;

              default:
                /* Any other flagged character produces a single space.  */
                putchar (' ');
              }
        }
      else
        putchar (*cursor);
    }
}

