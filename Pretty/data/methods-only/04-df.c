/* Like human_readable (N, BUF, human_output_opts, INPUT_UNITS, OUTPUT_UNITS),
   except:

    - If NEGATIVE, then N represents a negative number,
      expressed in two's complement.
    - Otherwise, return "-" if N is unknown.  */

static char const *
df_readable (bool negative, uintmax_t n, char *buf,
             uintmax_t input_units, uintmax_t output_units)
{
  if (! known_value (n) && !negative)
    return "-";
  else
    {
      char *p = human_readable (negative ? -n : n, buf + negative,
                                human_output_opts, input_units, output_units);
      if (negative)
        *--p = '-';
      return p;
    }
}

/* Logical equivalence */

