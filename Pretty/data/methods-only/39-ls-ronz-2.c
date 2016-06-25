  static int rev_strcmp_##key_name (V a, V b)				\
  { return key_cmp_func (b, a, strcmp); }				\

