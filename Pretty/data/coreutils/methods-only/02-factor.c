/* Emit the factors of the indicated number.  If we have the option of using
   either algorithm, we select on the basis of the length of the number.
   For longer numbers, we prefer the MP algorithm even if the native algorithm
   has enough digits, because the algorithm is better.  The turnover point
   depends on the value.  */
static bool
print_factors (char const *s)
{
  uintmax_t n;
  strtol_error err = xstrtoumax (s, NULL, 10, &n, "");

  enum { GMP_TURNOVER_POINT = 100000 };

  if (err == LONGINT_OVERFLOW
      || (err == LONGINT_OK && GMP_TURNOVER_POINT <= n))
    {
      mpz_t t;
      mpz_init (t);
      if (gmp_sscanf (s, "%Zd", t) == 1)
        {
          debug ("[%s]", _("using arbitrary-precision arithmetic"));
          print_factors_multi (t);
          return true;
        }
      err = LONGINT_INVALID;
    }

  switch (err)
    {
    case LONGINT_OK:
      debug ("[%s]", _("using single-precision arithmetic"));
      print_factors_single (n);
      return true;

    case LONGINT_OVERFLOW:
      error (0, 0, _("%s is too large"), quote (s));
      return false;

    default:
      error (0, 0, _("%s is not a valid positive integer"), quote (s));
      return false;
    }
}

enum
{
  VERBOSE_OPTION = CHAR_MAX + 1
};

static struct option const long_options[] =
{
  {"verbose", no_argument, NULL, VERBOSE_OPTION},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0}
};

