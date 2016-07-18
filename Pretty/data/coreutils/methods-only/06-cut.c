static bool
is_range_start_index (size_t i)
{
  return hash_lookup (range_start_ht, (void *) i) ? true : false;
}

