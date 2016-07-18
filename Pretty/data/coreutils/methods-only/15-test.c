static bool
two_arguments (void)
{
  bool value;

  if (STREQ (argv[pos], "!"))
    {
      advance (false);
      value = ! one_argument ();
    }
  else if (argv[pos][0] == '-'
           && argv[pos][1] != '\0'
           && argv[pos][2] == '\0')
    {
      if (test_unop (argv[pos]))
        value = unary_operator ();
      else
        test_syntax_error (_("%s: unary operator expected"), argv[pos]);
    }
  else
    beyond ();
  return (value);
}

