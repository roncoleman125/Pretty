/* If FILE (of length FILELEN) contains only portable characters,
   return true, else report an error and return false.  */

static bool
portable_chars_only (char const *file, size_t filelen)
{
  size_t validlen = strspn (file,
                            ("/"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "0123456789._-"));
  char const *invalid = file + validlen;

  if (*invalid)
    {
      DECLARE_ZEROED_AGGREGATE (mbstate_t, mbstate);
      size_t charlen = mbrlen (invalid, filelen - validlen, &mbstate);
      error (0, 0,
             _("nonportable character %s in file name %s"),
             quotearg_n_style_mem (1, locale_quoting_style, invalid,
                                   (charlen <= MB_LEN_MAX ? charlen : 1)),
             quote_n (0, file));
      return false;
    }

  return true;
}

