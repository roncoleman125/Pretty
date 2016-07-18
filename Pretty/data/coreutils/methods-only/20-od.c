/* STRINGS mode.  Find each "string constant" in the input.
   A string constant is a run of at least `string_min' ASCII
   graphic (or formatting) characters terminated by a null.
   Based on a function written by Richard Stallman for a
   traditional version of od.  Return true if successful.  */

static bool
dump_strings (void)
{
  size_t bufsize = MAX (100, string_min);
  char *buf = xmalloc (bufsize);
  uintmax_t address = n_bytes_to_skip;
  bool ok = true;

  while (1)
    {
      size_t i;
      int c;

      /* See if the next `string_min' chars are all printing chars.  */
    tryline:

      if (limit_bytes_to_format
          && (end_offset < string_min || end_offset - string_min <= address))
        break;

      for (i = 0; i < string_min; i++)
        {
          ok &= read_char (&c);
          address++;
          if (c < 0)
            {
              free (buf);
              return ok;
            }
          if (! isprint (c))
            /* Found a non-printing.  Try again starting with next char.  */
            goto tryline;
          buf[i] = c;
        }

      /* We found a run of `string_min' printable characters.
         Now see if it is terminated with a null byte.  */
      while (!limit_bytes_to_format || address < end_offset)
        {
          if (i == bufsize)
            {
              buf = X2REALLOC (buf, &bufsize);
            }
          ok &= read_char (&c);
          address++;
          if (c < 0)
            {
              free (buf);
              return ok;
            }
          if (c == '\0')
            break;		/* It is; print this string.  */
          if (! isprint (c))
            goto tryline;	/* It isn't; give up on this string.  */
          buf[i++] = c;		/* String continues; store it all.  */
        }

      /* If we get here, the string is all printable and null-terminated,
         so print it.  It is all in `buf' and `i' is its length.  */
      buf[i] = 0;
      format_address (address - i - 1, ' ');

      for (i = 0; (c = buf[i]); i++)
        {
          switch (c)
            {
            case '\a':
              fputs ("\\a", stdout);
              break;

            case '\b':
              fputs ("\\b", stdout);
              break;

            case '\f':
              fputs ("\\f", stdout);
              break;

            case '\n':
              fputs ("\\n", stdout);
              break;

            case '\r':
              fputs ("\\r", stdout);
              break;

            case '\t':
              fputs ("\\t", stdout);
              break;

            case '\v':
              fputs ("\\v", stdout);
              break;

            default:
              putc (c, stdout);
            }
        }
      putchar ('\n');
    }

  /* We reach this point only if we search through
     (max_bytes_to_format - string_min) bytes before reaching EOF.  */

  free (buf);

  ok &= check_and_close (0);
  return ok;
}

