static inline bool
is_printable_field (size_t i)
{
  size_t n = i / CHAR_BIT;
  return (printable_field[n] >> (i % CHAR_BIT)) & 1;
}

