static void
test_syntax_error (char const *format, char const *arg)
{
  fprintf (stderr, "%s: ", argv[0]);
  fprintf (stderr, format, arg);
  fputc ('\n', stderr);
  fflush (stderr);
  test_exit (TEST_FAILURE);
}

