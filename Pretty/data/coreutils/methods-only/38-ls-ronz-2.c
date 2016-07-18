  /* reverse, non-dirfirst versions */					\
  static int rev_xstrcoll_##key_name (V a, V b)				\
  { return key_cmp_func (b, a, xstrcoll); }				\
