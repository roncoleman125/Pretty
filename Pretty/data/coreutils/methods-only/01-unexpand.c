/* Add tab stop TABVAL to the end of `tab_list'.  */

static void
add_tab_stop (uintmax_t tabval)
{
  uintmax_t prev_column = first_free_tab ? tab_list[first_free_tab - 1] : 0;
  uintmax_t column_width = prev_column <= tabval ? tabval - prev_column : 0;

  if (first_free_tab == n_tabs_allocated)
    tab_list = X2NREALLOC (tab_list, &n_tabs_allocated);
  tab_list[first_free_tab++] = tabval;

  if (max_column_width < column_width)
    {
      if (SIZE_MAX < column_width)
        error (EXIT_FAILURE, 0, _("tabs are too far apart"));
      max_column_width = column_width;
    }
}

