/* Update `environ' for the new shell based on PW, with SHELL being
   the value for the SHELL environment variable.  */

static void
modify_environment (const struct passwd *pw, const char *shell)
{
  if (simulate_login)
    {
      /* Leave TERM unchanged.  Set HOME, SHELL, USER, LOGNAME, PATH.
         Unset all other environment variables.  */
      char const *term = getenv ("TERM");
      if (term)
        term = xstrdup (term);
      environ = xmalloc ((6 + !!term) * sizeof (char *));
      environ[0] = NULL;
      if (term)
        xsetenv ("TERM", term);
      xsetenv ("HOME", pw->pw_dir);
      xsetenv ("SHELL", shell);
      xsetenv ("USER", pw->pw_name);
      xsetenv ("LOGNAME", pw->pw_name);
      xsetenv ("PATH", (pw->pw_uid
                        ? DEFAULT_LOGIN_PATH
                        : DEFAULT_ROOT_LOGIN_PATH));
    }
  else
    {
      /* Set HOME, SHELL, and if not becoming a super-user,
         USER and LOGNAME.  */
      if (change_environment)
        {
          xsetenv ("HOME", pw->pw_dir);
          xsetenv ("SHELL", shell);
          if (pw->pw_uid)
            {
              xsetenv ("USER", pw->pw_name);
              xsetenv ("LOGNAME", pw->pw_name);
            }
        }
    }
}

