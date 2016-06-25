/*
 * expr:
 *	or
 */
static bool
expr (void)
{
  if (pos >= argc)
    beyond ();

  return or ();		/* Same with this. */
}

