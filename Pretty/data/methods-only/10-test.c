/*
 * and:
 *	term
 *	term '-a' and
 */
static bool
and (void)
{
  bool value = true;

  while (true)
    {
      value &= term ();
      if (! (pos < argc && STREQ (argv[pos], "-a")))
        return value;
      advance (false);
    }
}

