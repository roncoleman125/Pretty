/* Specify the number of threads to spawn during internal sort.  */
static size_t
specify_nthreads (int oi, char c, char const *s)
{
  unsigned long int nthreads;
  enum strtol_error e = xstrtoul (s, NULL, 10, &nthreads, "");
  if (e == LONGINT_OVERFLOW)
    return SIZE_MAX;
  if (e != LONGINT_OK)
    xstrtol_fatal (e, oi, c, long_options, s);
  if (SIZE_MAX < nthreads)
    nthreads = SIZE_MAX;
  if (nthreads == 0)
    error (SORT_FAILURE, 0, _("number in parallel must be nonzero"));
  return nthreads;
}


