static int xstrcoll_df_version (V a, V b)
{ DIRFIRST_CHECK (a, b); return cmp_version (a, b); }
