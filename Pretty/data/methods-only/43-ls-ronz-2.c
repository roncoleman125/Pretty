  static int rev_strcmp_df_##key_name (V a, V b)			\
  { DIRFIRST_CHECK (a, b); return key_cmp_func (b, a, strcmp); }

