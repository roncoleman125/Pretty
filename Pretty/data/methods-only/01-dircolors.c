/* If the SHELL environment variable is set to `csh' or `tcsh,'
   assume C shell.  Else Bourne shell.  */

static enum Shell_syntax
guess_shell_syntax (void)
{
  char *shell;

  shell = getenv ("SHELL");
  if (shell == NULL || *shell == '\0')
    return SHELL_SYNTAX_UNKNOWN;

  shell = last_component (shell);

  if (STREQ (shell, "csh") || STREQ (shell, "tcsh"))
    return SHELL_SYNTAX_C;

  return SHELL_SYNTAX_BOURNE;
}

