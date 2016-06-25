/* NOTE: identical to above, modulo use of cc_t */
static int
strtoul_cc_t (char const *s, int base, char **p, cc_t *result, char delim)
{
  unsigned long ul;
  errno = 0;
  ul = strtoul (s, p, base);
  if (errno || **p != delim || *p == s || (cc_t) ul != ul)
    return -1;
  *result = ul;
  return 0;
}

