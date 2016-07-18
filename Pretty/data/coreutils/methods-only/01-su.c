/* Ask the user for a password.
   Return true if the user gives the correct password for entry PW,
   false if not.  Return true without asking for a password if run by UID 0
   or if PW has an empty password.  */

static bool
correct_password (const struct passwd *pw)
{
  char *unencrypted, *encrypted, *correct;
  /* Shadow passwd stuff for SVR3 and maybe other systems.  */
  struct spwd *sp = getspnam (pw->pw_name);

  endspent ();
  if (sp)
    correct = sp->sp_pwdp;
  else
    correct = pw->pw_passwd;

  if (getuid () == 0 || !correct || correct[0] == '\0')
    return true;

  unencrypted = getpass (_("Password:"));
  if (!unencrypted)
    {
      error (0, 0, _("getpass: cannot open /dev/tty"));
      return false;
    }
  encrypted = crypt (unencrypted, correct);
  memset (unencrypted, 0, strlen (unencrypted));
  return STREQ (encrypted, correct);
}

