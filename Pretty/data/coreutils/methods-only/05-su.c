/* Return true if SHELL is a restricted shell (one not returned by
   getusershell), else false, meaning it is a standard shell.  */

static bool
restricted_shell (const char *shell)
{
  char *line;

  setusershell ();
  while ((line = getusershell ()) != NULL)
    {
      if (*line != '#' && STREQ (line, shell))
        {
          endusershell ();
          return false;
        }
    }
  endusershell ();
  return true;
}

