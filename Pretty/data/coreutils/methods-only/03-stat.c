/* PFORMAT points to a '%' followed by a prefix of a format, all of
   size PREFIX_LEN.  The flags allowed for this format are
   ALLOWED_FLAGS; remove other printf flags from the prefix, then
   append SUFFIX.  */
static void
make_format (char *pformat, size_t prefix_len, char const *allowed_flags,
             char const *suffix)
{
  char *dst = pformat + 1;
  char const *src;
  char const *srclim = pformat + prefix_len;
  for (src = dst; src < srclim && strchr (printf_flags, *src); src++)
    if (strchr (allowed_flags, *src))
      *dst++ = *src;
  while (src < srclim)
    *dst++ = *src++;
  strcpy (dst, suffix);
}

