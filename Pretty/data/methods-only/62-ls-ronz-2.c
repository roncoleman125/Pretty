/* Likewise, for groups.  */

static void
format_group (gid_t g, int width, bool stat_ok)
{
  format_user_or_group (! stat_ok ? "?" :
                        (numeric_ids ? NULL : getgroup (g)), g, width);
}

