/* Convert OPT to size_t, reporting an error using MSGID if OPT is
   invalid.  Silently convert too-large values to SIZE_MAX.  */

static size_t
size_opt (char const *opt, char const *msgid)
{
  unsigned long int size;
  verify (SIZE_MAX <= ULONG_MAX);

  switch (xstrtoul (opt, NULL, 10, &size, ""))
    {
    case LONGINT_OK:
    case LONGINT_OVERFLOW:
      break;

    default:
      error (EXIT_FAILURE, 0, "%s: %s", opt, _(msgid));
    }

  return MIN (size, SIZE_MAX);
}

