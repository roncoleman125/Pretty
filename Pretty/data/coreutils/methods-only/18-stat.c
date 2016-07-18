/* Print the information specified by the format string, FORMAT,
   calling PRINT_FUNC for each %-directive encountered.
   Return zero upon success, nonzero upon failure.  */
static bool ATTRIBUTE_WARN_UNUSED_RESULT
print_it (char const *format, char const *filename,
          bool (*print_func) (char *, size_t, unsigned int,
                              char const *, void const *),
          void const *data)
{
  bool fail = false;

  /* Add 2 to accommodate our conversion of the stat `%s' format string
     to the longer printf `%llu' one.  */
  enum
    {
      MAX_ADDITIONAL_BYTES =
        (MAX (sizeof PRIdMAX,
              MAX (sizeof PRIoMAX, MAX (sizeof PRIuMAX, sizeof PRIxMAX)))
         - 1)
    };
  size_t n_alloc = strlen (format) + MAX_ADDITIONAL_BYTES + 1;
  char *dest = xmalloc (n_alloc);
  char const *b;
  for (b = format; *b; b++)
    {
      switch (*b)
        {
        case '%':
          {
            size_t len = strspn (b + 1, printf_flags);
            char const *fmt_char = b + len + 1;
            fmt_char += strspn (fmt_char, digits);
            if (*fmt_char == '.')
              fmt_char += 1 + strspn (fmt_char + 1, digits);
            len = fmt_char - (b + 1);
            unsigned int fmt_code = *fmt_char;
            memcpy (dest, b, len + 1);

            b = fmt_char;
            switch (fmt_code)
              {
              case '\0':
                --b;
                /* fall through */
              case '%':
                if (0 < len)
                  {
                    dest[len + 1] = *fmt_char;
                    dest[len + 2] = '\0';
                    error (EXIT_FAILURE, 0, _("%s: invalid directive"),
                           quotearg_colon (dest));
                  }
                putchar ('%');
                break;
              default:
                fail |= print_func (dest, len + 1, fmt_code, filename, data);
                break;
              }
            break;
          }

        case '\\':
          if ( ! interpret_backslash_escapes)
            {
              putchar ('\\');
              break;
            }
          ++b;
          if (isodigit (*b))
            {
              int esc_value = octtobin (*b);
              int esc_length = 1;	/* number of octal digits */
              for (++b; esc_length < 3 && isodigit (*b);
                   ++esc_length, ++b)
                {
                  esc_value = esc_value * 8 + octtobin (*b);
                }
              putchar (esc_value);
              --b;
            }
          else if (*b == 'x' && isxdigit (to_uchar (b[1])))
            {
              int esc_value = hextobin (b[1]);	/* Value of \xhh escape. */
              /* A hexadecimal \xhh escape sequence must have
                 1 or 2 hex. digits.  */
              ++b;
              if (isxdigit (to_uchar (b[1])))
                {
                  ++b;
                  esc_value = esc_value * 16 + hextobin (*b);
                }
              putchar (esc_value);
            }
          else if (*b == '\0')
            {
              error (0, 0, _("warning: backslash at end of format"));
              putchar ('\\');
              /* Arrange to exit the loop.  */
              --b;
            }
          else
            {
              print_esc_char (*b);
            }
          break;

        default:
          putchar (*b);
          break;
        }
    }
  free (dest);

  fputs (trailing_delim, stdout);

  return fail;
}

