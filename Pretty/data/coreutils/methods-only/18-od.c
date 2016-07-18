/* If S is a valid traditional offset specification with an optional
   leading '+' return true and set *OFFSET to the offset it denotes.  */

static bool
parse_old_offset (const char *s, uintmax_t *offset)
{
  int radix;

  if (*s == '\0')
    return false;

  /* Skip over any leading '+'. */
  if (s[0] == '+')
    ++s;

  /* Determine the radix we'll use to interpret S.  If there is a `.',
     it's decimal, otherwise, if the string begins with `0X'or `0x',
     it's hexadecimal, else octal.  */
  if (strchr (s, '.') != NULL)
    radix = 10;
  else
    {
      if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        radix = 16;
      else
        radix = 8;
    }

  return xstrtoumax (s, NULL, radix, offset, "Bb") == LONGINT_OK;
}

