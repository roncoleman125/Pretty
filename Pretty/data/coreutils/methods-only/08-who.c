static void
print_login (const STRUCT_UTMP *utmp_ent)
{
  char *comment = make_id_equals_comment (utmp_ent);
  PIDSTR_DECL_AND_INIT (pidstr, utmp_ent);

  /* FIXME: add idle time? */

  print_line (-1, _("LOGIN"), ' ', sizeof utmp_ent->ut_line, utmp_ent->ut_line,
              time_string (utmp_ent), "", pidstr, comment, "");
  free (comment);
}

