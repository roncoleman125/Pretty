/* Return true if STR is of the form "PATTERN" or "PATTERNDELIM...".  */

static bool
operand_matches (char const *str, char const *pattern, char delim)
{
  while (*pattern)
    if (*str++ != *pattern++)
      return false;
  return !*str || *str == delim;
}

