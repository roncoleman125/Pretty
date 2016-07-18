static void
print_deadprocs (const STRUCT_UTMP *utmp_ent)
{
  static char *exitstr;
  char *comment = make_id_equals_comment (utmp_ent);
  PIDSTR_DECL_AND_INIT (pidstr, utmp_ent);

  if (!exitstr)
    exitstr = xmalloc (strlen (_("term="))
                       + INT_STRLEN_BOUND (UT_EXIT_E_TERMINATION (utmp_ent)) + 1
                       + strlen (_("exit="))
                       + INT_STRLEN_BOUND (UT_EXIT_E_EXIT (utmp_ent))
                       + 1);
  sprintf (exitstr, "%s%d %s%d", _("term="), UT_EXIT_E_TERMINATION (utmp_ent),
           _("exit="), UT_EXIT_E_EXIT (utmp_ent));

  /* FIXME: add idle time? */

  print_line (-1, "", ' ', sizeof utmp_ent->ut_line, utmp_ent->ut_line,
              time_string (utmp_ent), "", pidstr, comment, exitstr);
  free (comment);
}

