/* Convert the numeric group-id, GID, to a string stored in xmalloc'd memory,
   and return it.  If there's no corresponding group name, use the decimal
   representation of the ID.  */

extern char *
gid_to_name (gid_t gid)
{
  char buf[INT_BUFSIZE_BOUND (intmax_t)];
  struct group *grp = getgrgid (gid);
  return xstrdup (grp ? grp->gr_name
                  : TYPE_SIGNED (gid_t) ? imaxtostr (gid, buf)
                  : umaxtostr (gid, buf));
}

