/* Parse column count string S, and if it's valid (1 or larger and
   within range of the type of `columns') set the global variables
   columns and explicit_columns and return true.
   Otherwise, exit with a diagnostic.  */
static void
parse_column_count (char const *s)
{
  long int tmp_long;
  if (xstrtol (s, NULL, 10, &tmp_long, "") != LONGINT_OK
      || !(1 <= tmp_long && tmp_long <= INT_MAX))
    error (EXIT_FAILURE, 0,
           _("invalid number of columns: %s"), quote (s));

  columns = tmp_long;
  explicit_columns = true;
}

