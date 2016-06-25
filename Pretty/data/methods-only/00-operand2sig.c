extern int
operand2sig (char const *operand, char *signame)
{
  int signum;

  if (ISDIGIT (*operand))
    {
      char *endp;
      long int l = (errno = 0, strtol (operand, &endp, 10));
      int i = l;
      signum = (operand == endp || *endp || errno || i != l ? -1
                : WIFSIGNALED (i) ? WTERMSIG (i) : i);
    }
  else
    {
      /* Convert signal to upper case in the C locale, not in the
         current locale.  Don't assume ASCII; it might be EBCDIC.  */
      char *upcased = xstrdup (operand);
      char *p;
      for (p = upcased; *p; p++)
        if (strchr ("abcdefghijklmnopqrstuvwxyz", *p))
          *p += 'A' - 'a';

      /* Look for the signal name, possibly prefixed by "SIG",
         and possibly lowercased.  */
      if (!(str2sig (upcased, &signum) == 0
            || (upcased[0] == 'S' && upcased[1] == 'I' && upcased[2] == 'G'
                && str2sig (upcased + 3, &signum) == 0)))
        signum = -1;

      free (upcased);
    }

  if (signum < 0 || sig2str (signum, signame) != 0)
    {
      error (0, 0, _("%s: invalid signal"), operand);
      return -1;
    }

  return signum;
}
