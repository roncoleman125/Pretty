/*
 * beyond - call when we're beyond the end of the argument list (an
 *	error condition)
 */
static void
beyond (void)
{
  test_syntax_error (_("missing argument after %s"), quote (argv[argc - 1]));
}

