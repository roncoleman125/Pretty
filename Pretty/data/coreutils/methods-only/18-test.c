/*
 * [:
 *	'[' expr ']'
 * test:
 *	test expr
 */
int
main (int margc, char **margv)
{
  bool value;

  int code;

  code = setjmp (test_exit_buf);

  if (code)
    return (test_error_return);
  initialize_main (&margc, &margv);
  set_program_name (margv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (TEST_FAILURE);
  atexit (close_stdout);

  argv = margv;

  if (LBRACKET)
    {
      /* Recognize --help or --version, but only when invoked in the
         "[" form, when the last argument is not "]".  Use direct
         parsing, rather than parse_long_options, to avoid accepting
         abbreviations.  POSIX allows "[ --help" and "[ --version" to
         have the usual GNU behavior, but it requires "test --help"
         and "test --version" to exit silently with status 0.  */
      if (margc == 2)
        {
          if (STREQ (margv[1], "--help"))
            usage (EXIT_SUCCESS);

          if (STREQ (margv[1], "--version"))
            {
              version_etc (stdout, PROGRAM_NAME, PACKAGE_NAME, Version, AUTHORS,
                           (char *) NULL);
              test_exit (EXIT_SUCCESS);
            }
        }
      if (margc < 2 || !STREQ (margv[margc - 1], "]"))
        test_syntax_error (_("missing `]'"), NULL);

      --margc;
    }

  argc = margc;
  pos = 1;

  if (pos >= argc)
    test_exit (TEST_FALSE);

  value = posixtest (argc - 1);

  if (pos != argc)
    test_syntax_error (_("extra argument %s"), quote (argv[pos]));

  test_exit (value ? TEST_TRUE : TEST_FALSE);
}
