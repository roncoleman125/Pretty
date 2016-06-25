/* Single-precision factoring */
static void
print_factors_single (uintmax_t n)
{
  uintmax_t factors[MAX_N_FACTORS];
  size_t n_factors = factor_wheel (n, MAX_N_FACTORS, factors);
  size_t i;
  char buf[INT_BUFSIZE_BOUND (uintmax_t)];

  printf ("%s:", umaxtostr (n, buf));
  for (i = 0; i < n_factors; i++)
    printf (" %s", umaxtostr (factors[i], buf));
  putchar ('\n');
}

static int
mpcompare (const void *av, const void *bv)
{
  mpz_t *const *a = av;
  mpz_t *const *b = bv;
  return mpz_cmp (**a, **b);
}

static void
sort_and_print_factors (void)
{
  mpz_t **faclist;
  size_t i;

  faclist = xcalloc (nfactors_found, sizeof *faclist);
  for (i = 0; i < nfactors_found; ++i)
    {
      faclist[i] = &factor[i];
    }
  qsort (faclist, nfactors_found, sizeof *faclist, mpcompare);

  for (i = 0; i < nfactors_found; ++i)
    {
      fputc (' ', stdout);
      mpz_out_str (stdout, 10, *faclist[i]);
    }
  putchar ('\n');
  free (faclist);
}

static void
free_factors (void)
{
  size_t i;

  for (i = 0; i < nfactors_found; ++i)
    {
      mpz_clear (factor[i]);
    }
  /* Don't actually free factor[] because in the case where we are
     reading numbers from stdin, we may be about to use it again.  */
  nfactors_found = 0;
}

/* Arbitrary-precision factoring */
static void
print_factors_multi (mpz_t t)
{
  mpz_out_str (stdout, 10, t);
  putchar (':');

  if (mpz_sgn (t) != 0)
    {
      /* Set the trial division limit according to the size of t.  */
      size_t n_bits = mpz_sizeinbase (t, 2);
      unsigned int division_limit = MIN (n_bits, 1000);
      division_limit *= division_limit;

      factor_using_division (t, division_limit);

      if (mpz_cmp_ui (t, 1) != 0)
        {
          debug ("[is number prime?] ");
          if (mpz_probab_prime_p (t, 3))
            emit_factor (t);
          else
            factor_using_pollard_rho (t, 1);
        }
    }

  mpz_clear (t);
  sort_and_print_factors ();
  free_factors ();
}


