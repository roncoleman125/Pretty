static void
print_clockchange (const STRUCT_UTMP *utmp_ent)
{
  /* FIXME: handle NEW_TIME & OLD_TIME both */
  print_line (-1, "", ' ', -1, _("clock change"),
              time_string (utmp_ent), "", "", "", "");
}

