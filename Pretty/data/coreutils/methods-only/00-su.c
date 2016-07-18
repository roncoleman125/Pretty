/* Add NAME=VAL to the environment, checking for out of memory errors.  */

static void
xsetenv (char const *name, char const *val)
{
  size_t namelen = strlen (name);
  size_t vallen = strlen (val);
  char *string = xmalloc (namelen + 1 + vallen + 1);
  strcpy (string, name);
  string[namelen] = '=';
  strcpy (string + namelen + 1, val);
  if (putenv (string) != 0)
    xalloc_die ();
}

/* Log the fact that someone has run su to the user given by PW;
   if SUCCESSFUL is true, they gave the correct password, etc.  */

static void
log_su (struct passwd const *pw, bool successful)
{
  const char *new_user, *old_user, *tty;

  if (pw->pw_uid)
    return;
  new_user = pw->pw_name;
  /* The utmp entry (via getlogin) is probably the best way to identify
     the user, especially if someone su's from a su-shell.  */
  old_user = getlogin ();
  if (!old_user)
    {
      /* getlogin can fail -- usually due to lack of utmp entry.
         Resort to getpwuid.  */
      struct passwd *pwd = getpwuid (getuid ());
      old_user = (pwd ? pwd->pw_name : "");
    }
  tty = ttyname (STDERR_FILENO);
  if (!tty)
    tty = "none";
  /* 4.2BSD openlog doesn't have the third parameter.  */
  openlog (last_component (program_name), 0
           , LOG_AUTH
           );
  syslog (LOG_NOTICE,
          "%s(to %s) %s on %s",
          "%s%s on %s",
          successful ? "" : "FAILED SU ",
          new_user,
          old_user, tty);
  closelog ();
}

