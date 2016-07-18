/* Convert a string of decimal digits, STR (the 1-based join field number),
   to an integral value.  Upon successful conversion, return one less
   (the zero-based field number).  Silently convert too-large values
   to SIZE_MAX - 1.  Otherwise, if a value cannot be converted, give a
   diagnostic and exit.  */

static size_t
string_to_join_field (char const *str)
{
  size_t result;
  unsigned long int val;
  verify (SIZE_MAX <= ULONG_MAX);

  strtol_error s_err = xstrtoul (str, NULL, 10, &val, "");
  if (s_err == LONGINT_OVERFLOW || (s_err == LONGINT_OK && SIZE_MAX < val))
    val = SIZE_MAX;
  else if (s_err != LONGINT_OK || val == 0)
    error (EXIT_FAILURE, 0, _("invalid field number: %s"), quote (str));

  result = val - 1;

  return result;
}

