/* NOTE: identical to below, modulo use of tcflag_t */
static int
strtoul_tcflag_t (char const *s, int base, char **p, tcflag_t *result,
                  char delim)
{
  unsigned long ul;
  errno = 0;
  ul = strtoul (s, p, base);
  if (errno || **p != delim || *p == s || (tcflag_t) ul != ul)
    return -1;
  *result = ul;
  return 0;
}

