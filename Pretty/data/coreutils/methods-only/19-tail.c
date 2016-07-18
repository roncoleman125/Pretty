/* If obsolete usage is allowed, and the command line arguments are of
   the obsolete form and the option string is well-formed, set
   *N_UNITS, the globals COUNT_LINES, FOREVER, and FROM_START, and
   return true.  If the command line arguments are obviously incorrect
   (e.g., because obsolete usage is not allowed and the arguments are
   incorrect for non-obsolete usage), report an error and exit.
   Otherwise, return false and don't modify any parameter or global
   variable.  */

static bool
parse_obsolete_option (int argc, char * const *argv, uintmax_t *n_units)
{
  const char *p;
  const char *n_string;
  const char *n_string_end;
  bool obsolete_usage;
  int default_count = DEFAULT_N_LINES;
  bool t_from_start;
  bool t_count_lines = true;
  bool t_forever = false;

  /* With the obsolete form, there is one option string and at most
     one file argument.  Watch out for "-" and "--", though.  */
  if (! (argc == 2
         || (argc == 3 && ! (argv[2][0] == '-' && argv[2][1]))
         || (3 <= argc && argc <= 4 && STREQ (argv[2], "--"))))
    return false;

  obsolete_usage = (posix2_version () < 200112);
  p = argv[1];

  switch (*p++)
    {
    default:
      return false;

    case '+':
      /* Leading "+" is a file name in the non-obsolete form.  */
      if (!obsolete_usage)
        return false;

      t_from_start = true;
      break;

    case '-':
      /* In the non-obsolete form, "-" is standard input and "-c"
         requires an option-argument.  The obsolete multidigit options
         are supported as a GNU extension even when conforming to
         POSIX 1003.1-2001, so don't complain about them.  */
      if (!obsolete_usage && !p[p[0] == 'c'])
        return false;

      t_from_start = false;
      break;
    }

  n_string = p;
  while (ISDIGIT (*p))
    p++;
  n_string_end = p;

  switch (*p)
    {
    case 'b': default_count *= 512;	/* Fall through.  */
    case 'c': t_count_lines = false;	/* Fall through.  */
    case 'l': p++; break;
    }

  if (*p == 'f')
    {
      t_forever = true;
      ++p;
    }

  if (*p)
    return false;

  if (n_string == n_string_end)
    *n_units = default_count;
  else if ((xstrtoumax (n_string, NULL, 10, n_units, "b")
            & ~LONGINT_INVALID_SUFFIX_CHAR)
           != LONGINT_OK)
    error (EXIT_FAILURE, 0, _("number in %s is too large"), quote (argv[1]));

  /* Set globals.  */
  from_start = t_from_start;
  count_lines = t_count_lines;
  forever = t_forever;

  return true;
}

