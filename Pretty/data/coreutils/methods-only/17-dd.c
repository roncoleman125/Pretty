/* Return the value of STR, interpreted as a non-negative decimal integer,
   optionally multiplied by various values.
   Set *INVALID if STR does not represent a number in this format.  */

static uintmax_t
parse_integer (const char *str, bool *invalid)
{
  uintmax_t n;
  char *suffix;
  enum strtol_error e = xstrtoumax (str, &suffix, 10, &n, "bcEGkKMPTwYZ0");

  if (e == LONGINT_INVALID_SUFFIX_CHAR && *suffix == 'x')
    {
      uintmax_t multiplier = parse_integer (suffix + 1, invalid);

      if (multiplier != 0 && n * multiplier / multiplier != n)
        {
          *invalid = true;
          return 0;
        }

      n *= multiplier;
    }
  else if (e != LONGINT_OK)
    {
      *invalid = true;
      return 0;
    }

  return n;
}

