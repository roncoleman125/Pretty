/* Become the user and group(s) specified by PW.  */

static void
change_identity (const struct passwd *pw)
{
  errno = 0;
  if (initgroups (pw->pw_name, pw->pw_gid) == -1)
    error (EXIT_CANCELED, errno, _("cannot set groups"));
  endgrent ();
  if (setgid (pw->pw_gid))
    error (EXIT_CANCELED, errno, _("cannot set group id"));
  if (setuid (pw->pw_uid))
    error (EXIT_CANCELED, errno, _("cannot set user id"));
}

