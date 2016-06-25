static char *
make_id_equals_comment (STRUCT_UTMP const *utmp_ent)
{
  char *comment = xmalloc (strlen (_("id=")) + sizeof UT_ID (utmp_ent) + 1);

  strcpy (comment, _("id="));
  strncat (comment, UT_ID (utmp_ent), sizeof UT_ID (utmp_ent));
  return comment;
}

