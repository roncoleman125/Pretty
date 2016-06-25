/* Likewise, for groups.  */

static int
format_group_width (gid_t g)
{
  return format_user_or_group_width (numeric_ids ? NULL : getgroup (g), g);
}

