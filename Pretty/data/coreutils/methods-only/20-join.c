/* Convert a single field specifier string, S, to a *FILE_INDEX, *FIELD_INDEX
   pair.  In S, the field index string is 1-based; *FIELD_INDEX is zero-based.
   If S is valid, return true.  Otherwise, give a diagnostic and exit.  */

static void
decode_field_spec (const char *s, int *file_index, size_t *field_index)
{
  /* The first character must be 0, 1, or 2.  */
  switch (s[0])
    {
    case '0':
      if (s[1])
        {
          /* `0' must be all alone -- no `.FIELD'.  */
          error (EXIT_FAILURE, 0, _("invalid field specifier: %s"), quote (s));
        }
      *file_index = 0;
      *field_index = 0;
      break;

    case '1':
    case '2':
      if (s[1] != '.')
        error (EXIT_FAILURE, 0, _("invalid field specifier: %s"), quote (s));
      *file_index = s[0] - '0';
      *field_index = string_to_join_field (s + 2);
      break;

    default:
      error (EXIT_FAILURE, 0,
             _("invalid file number in field spec: %s"), quote (s));

      /* Tell gcc -W -Wall that we can't get beyond this point.
         This avoids a warning (otherwise legit) that the caller's copies
         of *file_index and *field_index might be used uninitialized.  */
      abort ();

      break;
    }
}

