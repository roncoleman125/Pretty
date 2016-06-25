int
main (int argc, char **argv)
{
  VALUE *v;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  initialize_exit_failure (EXPR_FAILURE);
  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE_NAME, VERSION,
                      usage, AUTHORS, (char const *) NULL);
  /* The above handles --help and --version.
     Since there is no other invocation of getopt, handle `--' here.  */
  if (argc > 1 && STREQ (argv[1], "--"))
    {
      --argc;
      ++argv;
    }

  if (argc <= 1)
    {
      error (0, 0, _("missing operand"));
      usage (EXPR_INVALID);
    }

  args = argv + 1;

  v = eval (true);
  if (!nomoreargs ())
    syntax_error ();
  printv (v);

  exit (null (v));
}

