/* Check that the list of tab stops TABS, with ENTRIES entries,
   contains only nonzero, ascending values.  */

static void
validate_tab_stops (uintmax_t const *tabs, size_t entries)
{
  uintmax_t prev_tab = 0;
  size_t i;

  for (i = 0; i < entries; i++)
    {
      if (tabs[i] == 0)
        error (EXIT_FAILURE, 0, _("tab size cannot be 0"));
      if (tabs[i] <= prev_tab)
        error (EXIT_FAILURE, 0, _("tab sizes must be ascending"));
      prev_tab = tabs[i];
    }
}

