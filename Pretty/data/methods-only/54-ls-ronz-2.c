static int rev_xstrcoll_df_version (V a, V b)
{ DIRFIRST_CHECK (a, b); return cmp_version (b, a); }

