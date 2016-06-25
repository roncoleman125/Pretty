/* Output all the fields in line, other than the join field.  */

static void
prfields (struct line const *line, size_t join_field, size_t autocount)
{
  size_t i;
  size_t nfields = autoformat ? autocount : line->nfields;
  char output_separator = tab < 0 ? ' ' : tab;

  for (i = 0; i < join_field && i < nfields; ++i)
    {
      putchar (output_separator);
      prfield (i, line);
    }
  for (i = join_field + 1; i < nfields; ++i)
    {
      putchar (output_separator);
      prfield (i, line);
    }
}

