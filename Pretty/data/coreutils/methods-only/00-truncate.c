/* Set size to the value of STR, interpreted as a decimal integer,
   optionally multiplied by various values.
   Return -1 on error, 0 on success.

   This supports dd BLOCK size suffixes + lowercase g,t,m for bsd compat
   Note we don't support dd's b=512, c=1, w=2 or 21x512MiB formats.  */
static int
parse_len (char const *str, off_t *size)
{
  enum strtol_error e;
  intmax_t tmp_size;
  e = xstrtoimax (str, NULL, 10, &tmp_size, "EgGkKmMPtTYZ0");
  if (e == LONGINT_OK
      && !(OFF_T_MIN <= tmp_size && tmp_size <= OFF_T_MAX))
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

