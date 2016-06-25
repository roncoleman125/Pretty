static void
print_runlevel (const STRUCT_UTMP *utmp_ent)
{
  static char *runlevline, *comment;
  unsigned char last = UT_PID (utmp_ent) / 256;
  unsigned char curr = UT_PID (utmp_ent) % 256;

  if (!runlevline)
    runlevline = xmalloc (strlen (_("run-level")) + 3);
  sprintf (runlevline, "%s %c", _("run-level"), curr);

  if (!comment)
    comment = xmalloc (strlen (_("last=")) + 2);
  sprintf (comment, "%s%c", _("last="), (last == 'N') ? 'S' : last);

  print_line (-1, "", ' ', -1, runlevline, time_string (utmp_ent),
              "", "", c_isprint (last) ? comment : "", "");

  return;
}

