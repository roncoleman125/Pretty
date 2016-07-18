/* Return true if there no more tokens.  */

static bool
nomoreargs (void)
{
  return *args == 0;
}

/* Print evaluation trace and args remaining.  */

static void
trace (fxn)
     char *fxn;
{
  char **a;

  printf ("%s:", fxn);
  for (a = args; *a; a++)
    printf (" %s", *a);
  putchar ('\n');
}

