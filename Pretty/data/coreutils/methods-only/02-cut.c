static inline void
mark_printable_field (size_t i)
{
  size_t n = i / CHAR_BIT;
  printable_field[n] |= (1 << (i % CHAR_BIT));
}

