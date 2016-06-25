/* Print the text in FORMAT, using ARGV (with ARGC elements) for
   arguments to any `%' directives.
   Return the number of elements of ARGV used.  */

static int
print_formatted (const char *format, int argc, char **argv)
{
  int save_argc = argc;		/* Preserve original value.  */
  const char *f;		/* Pointer into `format'.  */
  const char *direc_start;	/* Start of % directive.  */
  size_t direc_length;		/* Length of % directive.  */
  bool have_field_width;	/* True if FIELD_WIDTH is valid.  */
  int field_width = 0;		/* Arg to first '*'.  */
  bool have_precision;		/* True if PRECISION is valid.  */
  int precision = 0;		/* Arg to second '*'.  */
  char ok[UCHAR_MAX + 1];	/* ok['x'] is true if %x is allowed.  */

  for (f = format; *f; ++f)
    {
      switch (*f)
        {
        case '%':
          direc_start = f++;
          direc_length = 1;
          have_field_width = have_precision = false;
          if (*f == '%')
            {
              putchar ('%');
              break;
            }
          if (*f == 'b')
            {
              /* FIXME: Field width and precision are not supported
                 for %b, even though POSIX requires it.  */
              if (argc > 0)
                {
                  print_esc_string (*argv);
                  ++argv;
                  --argc;
                }
              break;
            }

          memset (ok, 0, sizeof ok);
          ok['a'] = ok['A'] = ok['c'] = ok['d'] = ok['e'] = ok['E'] =
            ok['f'] = ok['F'] = ok['g'] = ok['G'] = ok['i'] = ok['o'] =
            ok['s'] = ok['u'] = ok['x'] = ok['X'] = 1;

          for (;; f++, direc_length++)
            switch (*f)
              {
              case 'I':
              case '\'':
                ok['a'] = ok['A'] = ok['c'] = ok['e'] = ok['E'] =
                  ok['o'] = ok['s'] = ok['x'] = ok['X'] = 0;
                break;
              case '-': case '+': case ' ':
                break;
              case '#':
                ok['c'] = ok['d'] = ok['i'] = ok['s'] = ok['u'] = 0;
                break;
              case '0':
                ok['c'] = ok['s'] = 0;
                break;
              default:
                goto no_more_flag_characters;
              }
        no_more_flag_characters:;

          if (*f == '*')
            {
              ++f;
              ++direc_length;
              if (argc > 0)
                {
                  intmax_t width = vstrtoimax (*argv);
                  if (INT_MIN <= width && width <= INT_MAX)
                    field_width = width;
                  else
                    error (EXIT_FAILURE, 0, _("invalid field width: %s"),
                           *argv);
                  ++argv;
                  --argc;
                }
              else
                field_width = 0;
              have_field_width = true;
            }
          else
            while (ISDIGIT (*f))
              {
                ++f;
                ++direc_length;
              }
          if (*f == '.')
            {
              ++f;
              ++direc_length;
              ok['c'] = 0;
              if (*f == '*')
                {
                  ++f;
                  ++direc_length;
                  if (argc > 0)
                    {
                      intmax_t prec = vstrtoimax (*argv);
                      if (prec < 0)
                        {
                          /* A negative precision is taken as if the
                             precision were omitted, so -1 is safe
                             here even if prec < INT_MIN.  */
                          precision = -1;
                        }
                      else if (INT_MAX < prec)
                        error (EXIT_FAILURE, 0, _("invalid precision: %s"),
                               *argv);
                      else
                        precision = prec;
                      ++argv;
                      --argc;
                    }
                  else
                    precision = 0;
                  have_precision = true;
                }
              else
                while (ISDIGIT (*f))
                  {
                    ++f;
                    ++direc_length;
                  }
            }

          while (*f == 'l' || *f == 'L' || *f == 'h'
                 || *f == 'j' || *f == 't' || *f == 'z')
            ++f;

          {
            unsigned char conversion = *f;
            if (! ok[conversion])
              error (EXIT_FAILURE, 0,
                     _("%.*s: invalid conversion specification"),
                     (int) (f + 1 - direc_start), direc_start);
          }

          print_direc (direc_start, direc_length, *f,
                       have_field_width, field_width,
                       have_precision, precision,
                       (argc <= 0 ? "" : (argc--, *argv++)));
          break;

        case '\\':
          f += print_esc (f, false);
          break;

        default:
          putchar (*f);
        }
    }

  return save_argc - argc;
}

