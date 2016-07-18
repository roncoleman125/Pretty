  static int strcmp_##key_name (V a, V b)				\
  { return key_cmp_func (a, b, strcmp); }				\

