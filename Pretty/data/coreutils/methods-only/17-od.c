/* Return the least common multiple of the sizes associated
   with the format specs.  */

static int
get_lcm (void)
{
  size_t i;
  int l_c_m = 1;

  for (i = 0; i < n_specs; i++)
    l_c_m = lcm (l_c_m, width_bytes[spec[i].size]);
  return l_c_m;
}

