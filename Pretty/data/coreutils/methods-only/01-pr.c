/* Estimate first_ / last_page_number
   using option +FIRST_PAGE:LAST_PAGE */

static bool
first_last_page (int oi, char c, char const *pages)
{
  char *p;
  uintmax_t first;
  uintmax_t last = UINTMAX_MAX;
  strtol_error err = xstrtoumax (pages, &p, 10, &first, "");
  if (err != LONGINT_OK && err != LONGINT_INVALID_SUFFIX_CHAR)
    xstrtol_fatal (err, oi, c, long_options, pages);

  if (p == pages || !first)
    return false;

  if (*p == ':')
    {
      char const *p1 = p + 1;
      err = xstrtoumax (p1, &p, 10, &last, "");
      if (err != LONGINT_OK)
        xstrtol_fatal (err, oi, c, long_options, pages);
      if (p1 == p || last < first)
        return false;
    }

  if (*p)
    return false;

  first_page_number = first;
  last_page_number = last;
  return true;
}

