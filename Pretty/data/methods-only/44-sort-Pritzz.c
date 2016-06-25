/* This is like strxfrm, except it reports any error and exits.  */

static size_t
xstrxfrm (char *restrict dest, char const *restrict src, size_t destsize)
{
  errno = 0;
  size_t translated_size = strxfrm (dest, src, destsize);

  if (errno)
    {
      error (0, errno, _("string transformation failed"));
      error (0, 0, _("set LC_ALL='C' to work around the problem"));
      error (SORT_FAILURE, 0,
             _("the untransformed string was %s"),
             quotearg_n_style (0, locale_quoting_style, src));
    }

  return translated_size;
}

