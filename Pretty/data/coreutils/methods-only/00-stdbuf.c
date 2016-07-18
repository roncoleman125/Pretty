/* Set size to the value of STR, interpreted as a decimal integer,
   optionally multiplied by various values.
   Return -1 on error, 0 on success.

   This supports dd BLOCK size suffixes.
   Note we don't support dd's b=512, c=1, w=2 or 21x512MiB formats.  */
static int
parse_size (char const *str, size_t *size)
{
  uintmax_t tmp_size;
  enum strtol_error e = xstrtoumax (str, NULL, 10, &tmp_size, "EGkKMPTYZ0");
  if (e == LONGINT_OK && tmp_size > SIZE_MAX)
    e = LONGINT_OVERFLOW;

  if (e == LONGINT_OK)
    {
      errno = 0;
      *size = tmp_size;
      return 0;
    }

  errno = (e == LONGINT_OVERFLOW ? EOVERFLOW : 0);
  return -1;
}

