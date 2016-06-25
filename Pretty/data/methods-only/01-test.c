/* Increment our position in the argument list.  Check that we're not
   past the end of the argument list.  This check is supressed if the
   argument is false.  */

static void
advance (bool f)
{
  ++pos;

  if (f && pos >= argc)
    beyond ();
}

