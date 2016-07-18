/*
 * term - parse a term and return 1 or 0 depending on whether the term
 *	evaluates to true or false, respectively.
 *
 * term ::=
 *	'-'('h'|'d'|'f'|'r'|'s'|'w'|'c'|'b'|'p'|'u'|'g'|'k') filename
 *	'-'('L'|'x') filename
 *	'-t' int
 *	'-'('z'|'n') string
 *	string
 *	string ('!='|'=') string
 *	<int> '-'(eq|ne|le|lt|ge|gt) <int>
 *	file '-'(nt|ot|ef) file
 *	'(' <expr> ')'
 * int ::=
 *	'-l' string
 *	positive and negative integers
 */
static bool
term (void)
{
  bool value;
  bool negated = false;

  /* Deal with leading `not's.  */
  while (pos < argc && argv[pos][0] == '!' && argv[pos][1] == '\0')
    {
      advance (true);
      negated = !negated;
    }

  if (pos >= argc)
    beyond ();

  /* A paren-bracketed argument. */
  if (argv[pos][0] == '(' && argv[pos][1] == '\0')
    {
      int nargs;

      advance (true);

      for (nargs = 1;
           pos + nargs < argc && ! STREQ (argv[pos + nargs], ")");
           nargs++)
        if (nargs == 4)
          {
            nargs = argc - pos;
            break;
          }

      value = posixtest (nargs);
      if (argv[pos] == 0)
        test_syntax_error (_("')' expected"), NULL);
      else
        if (argv[pos][0] != ')' || argv[pos][1])
          test_syntax_error (_("')' expected, found %s"), argv[pos]);
      advance (false);
    }

  /* Are there enough arguments left that this could be dyadic?  */
  else if (4 <= argc - pos && STREQ (argv[pos], "-l") && binop (argv[pos + 2]))
    value = binary_operator (true);
  else if (3 <= argc - pos && binop (argv[pos + 1]))
    value = binary_operator (false);

  /* It might be a switch type argument.  */
  else if (argv[pos][0] == '-' && argv[pos][1] && argv[pos][2] == '\0')
    {
      if (test_unop (argv[pos]))
        value = unary_operator ();
      else
        test_syntax_error (_("%s: unary operator expected"), argv[pos]);
    }
  else
    {
      value = (argv[pos][0] != '\0');
      advance (false);
    }

  return negated ^ value;
}

