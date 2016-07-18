/* Print the name or id of the user with id U, using a print width of
   WIDTH.  */

static void
format_user (uid_t u, int width, bool stat_ok)
{
  format_user_or_group (! stat_ok ? "?" :
                        (numeric_ids ? NULL : getuser (u)), u, width);
}

