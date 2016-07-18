static int
general_numcompare (char const *sa, char const *sb)
{
  /* FIXME: maybe add option to try expensive FP conversion
     only if A and B can't be compared more cheaply/accurately.  */

  char *ea;
  char *eb;
  long_double a = strtold (sa, &ea);
  long_double b = strtold (sb, &eb);

  /* Put conversion errors at the start of the collating sequence.  */
  if (sa == ea)
    return sb == eb ? 0 : -1;
  if (sb == eb)
    return 1;

  /* Sort numbers in the usual way, where -0 == +0.  Put NaNs after
     conversion errors but before numbers; sort them by internal
     bit-pattern, for lack of a more portable alternative.  */
  return (a < b ? -1
          : a > b ? 1
          : a == b ? 0
          : b == b ? -1
          : a == a ? 1
          : memcmp (&a, &b, sizeof a));
}

