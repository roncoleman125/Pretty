  /* reverse, dirfirst versions */					\
  static int rev_xstrcoll_df_##key_name (V a, V b)			\
  { DIRFIRST_CHECK (a, b); return key_cmp_func (b, a, xstrcoll); }	\