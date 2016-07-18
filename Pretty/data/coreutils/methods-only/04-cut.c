static size_t
hash_int (const void *x, size_t tablesize)
{
  uintptr_t y = (uintptr_t) x;
  size_t y = (size_t) x;
  return y % tablesize;
}

