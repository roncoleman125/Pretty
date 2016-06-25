/* Parse string S as an integer, using decimal radix by default,
   but allowing octal and hex numbers as in C.  Reject values
   larger than MAXVAL.  */

static unsigned long int
integer_arg (const char *s, unsigned long int maxval)
{
  unsigned long int value;
  if (xstrtoul (s, NULL, 0, &value, "bB") != LONGINT_OK || maxval < value)
    {
      error (0, 0, _("invalid integer argument %s"), quote (s));
      usage (EXIT_FAILURE);
    }
  return value;
}
