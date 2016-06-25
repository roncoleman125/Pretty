static bool
three_arguments (void)
{
  bool value;

  if (binop (argv[pos + 1]))
    value = binary_operator (false);
  else if (STREQ (argv[pos], "!"))
    {
      advance (true);
      value = !two_arguments ();
    }
  else if (STREQ (argv[pos], "(") && STREQ (argv[pos + 2], ")"))
    {
      advance (false);
      value = one_argument ();
      advance (false);
    }
  else if (STREQ (argv[pos + 1], "-a") || STREQ (argv[pos + 1], "-o"))
    value = expr ();
  else
    test_syntax_error (_("%s: binary operator expected"), argv[pos+1]);
  return (value);
}

