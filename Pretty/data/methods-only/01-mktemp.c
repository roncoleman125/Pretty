static size_t
count_consecutive_X_s (const char *s, size_t len)
{
  size_t n = 0;
  for ( ; len && s[len-1] == 'X'; len--)
    ++n;
  return n;
}

