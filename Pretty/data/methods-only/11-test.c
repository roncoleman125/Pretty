/*
 * or:
 *	and
 *	and '-o' or
 */
static bool
or (void)
{
  bool value = false;

  while (true)
    {
      value |= and ();
      if (! (pos < argc && STREQ (argv[pos], "-o")))
        return value;
      advance (false);
    }
}

