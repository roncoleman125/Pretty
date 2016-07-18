/* Extract a size_t value from a integer value I.
   If the value is negative, return SIZE_MAX.
   If the value is too large, return SIZE_MAX - 1.  */
static size_t
getsize (mpz_t i)
{
  if (mpz_sgn (i) < 0)
    return SIZE_MAX;
  if (mpz_fits_ulong_p (i))
    {
      unsigned long int ul = mpz_get_ui (i);
      if (ul < SIZE_MAX)
        return ul;
    }
  return SIZE_MAX - 1;
}

