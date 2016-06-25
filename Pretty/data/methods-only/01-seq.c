/* Read a long double value from the command line.
   Return if the string is correct else signal error.  */

static operand
scan_arg (const char *arg)
{
  operand ret;

  if (! xstrtold (arg, NULL, &ret.value, c_strtold))
    {
      error (0, 0, _("invalid floating point argument: %s"), arg);
      usage (EXIT_FAILURE);
    }

  /* We don't output spaces or '+' so don't include in width */
  while (isspace (to_uchar (*arg)) || *arg == '+')
    arg++;

  ret.width = strlen (arg);
  ret.precision = INT_MAX;

  if (! arg[strcspn (arg, "xX")] && isfinite (ret.value))
    {
      char const *decimal_point = strchr (arg, '.');
      if (! decimal_point)
        ret.precision = 0;
      else
        {
          size_t fraction_len = strcspn (decimal_point + 1, "eE");
          if (fraction_len <= INT_MAX)
            ret.precision = fraction_len;
          ret.width += (fraction_len == 0                      /* #.  -> #   */
                        ? -1
                        : (decimal_point == arg                /* .#  -> 0.# */
                           || ! ISDIGIT (decimal_point[-1]))); /* -.# -> 0.# */
        }
      char const *e = strchr (arg, 'e');
      if (! e)
        e = strchr (arg, 'E');
      if (e)
        {
          long exponent = strtol (e + 1, NULL, 10);
          ret.precision += exponent < 0 ? -exponent : 0;
        }
    }

  return ret;
}

