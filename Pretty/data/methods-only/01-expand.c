/* Add tab stop TABVAL to the end of `tab_list'.  */

static void
add_tab_stop (uintmax_t tabval)
{
  if (first_free_tab == n_tabs_allocated)
    tab_list = X2NREALLOC (tab_list, &n_tabs_allocated);
  tab_list[first_free_tab++] = tabval;
}

