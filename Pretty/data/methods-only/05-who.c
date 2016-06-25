static void
print_boottime (const STRUCT_UTMP *utmp_ent)
{
  print_line (-1, "", ' ', -1, _("system boot"),
              time_string (utmp_ent), "", "", "", "");
}

