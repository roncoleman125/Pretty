  /* direct, non-dirfirst versions */					\
  static int xstrcoll_##key_name (V a, V b)				\
  { return key_cmp_func (a, b, xstrcoll); }				\
