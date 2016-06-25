/* Return the size of the file name component F.  F must be nonempty.  */

static size_t
component_len (char const *f)
{
  size_t len;
  for (len = 1; f[len] != '/' && f[len]; len++)
    continue;
  return len;
}

