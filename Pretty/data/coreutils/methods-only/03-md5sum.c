/* Return true if S is a NUL-terminated string of DIGEST_HEX_BYTES hex digits.
   Otherwise, return false.  */
static bool
hex_digits (unsigned char const *s)
{
  unsigned int i;
  for (i = 0; i < digest_hex_bytes; i++)
    {
      if (!isxdigit (*s))
        return false;
      ++s;
    }
  return *s == '\0';
}

