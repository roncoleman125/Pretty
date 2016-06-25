/* FIXME: comment */

static size_t
factor_wheel (uintmax_t n0, size_t max_n_factors, uintmax_t *factors)
{
  uintmax_t n = n0, d, q;
  size_t n_factors = 0;
  unsigned char const *w = wheel_tab;

  if (n <= 1)
    return n_factors;

  /* The exit condition in the following loop is correct because
     any time it is tested one of these 3 conditions holds:
      (1) d divides n
      (2) n is prime
      (3) n is composite but has no factors less than d.
     If (1) or (2) obviously the right thing happens.
     If (3), then since n is composite it is >= d^2. */

  d = 2;
  do
    {
      q = n / d;
      while (n == q * d)
        {
          assert (n_factors < max_n_factors);
          factors[n_factors++] = d;
          n = q;
          q = n / d;
        }
      d += *(w++);
      if (w == WHEEL_END)
        w = WHEEL_START;
    }
  while (d <= q);

  if (n != 1 || n0 == 1)
    {
      assert (n_factors < max_n_factors);
      factors[n_factors++] = n;
    }

  return n_factors;
}

