/* Print the username of each valid entry and the number of valid entries
   in UTMP_BUF, which should have N elements. */

static void
print_heading (void)
{
  printf ("%-8s", _("Login"));
  if (include_fullname)
    printf (" %-19s", _("Name"));
  printf (" %-9s", _(" TTY"));
  if (include_idle)
    printf (" %-6s", _("Idle"));
  printf (" %-*s", time_format_width, _("When"));
  if (include_where)
    printf (" %s", _("Where"));
  putchar ('\n');
}

