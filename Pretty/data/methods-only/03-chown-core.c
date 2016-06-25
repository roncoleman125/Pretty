/* Convert the numeric user-id, UID, to a string stored in xmalloc'd memory,
   and return it.  If there's no corresponding user name, use the decimal
   representation of the ID.  */

extern char *
uid_to_name (uid_t uid)
{
  char buf[INT_BUFSIZE_BOUND (intmax_t)];
  struct passwd *pwd = getpwuid (uid);
  return xstrdup (pwd ? pwd->pw_name
                  : TYPE_SIGNED (uid_t) ? imaxtostr (uid, buf)
                  : umaxtostr (uid, buf));
}

