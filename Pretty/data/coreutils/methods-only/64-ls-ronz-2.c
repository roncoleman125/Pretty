/* Return the number of columns that format_user will print.  */

static int
format_user_width (uid_t u)
{
  return format_user_or_group_width (numeric_ids ? NULL : getuser (u), u);
}

