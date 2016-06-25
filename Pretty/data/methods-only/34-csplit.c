/* Given that the first character of command line arg STR is '{',
   make sure that the rest of the string is a valid repeat count
   and store its value in P.
   ARGNUM is the ARGV index of STR. */

static void
parse_repeat_count (int argnum, struct control *p, char *str)
{
  uintmax_t val;
  char *end;

  end = str + strlen (str) - 1;
  if (*end != '}')
    error (EXIT_FAILURE, 0, _("%s: `}' is required in repeat count"), str);
  *end = '\0';

  if (str+1 == end-1 && *(str+1) == '*')
    p->repeat_forever = true;
  else
    {
      if (xstrtoumax (str + 1, NULL, 10, &val, "") != LONGINT_OK)
        {
          error (EXIT_FAILURE, 0,
                 _("%s}: integer required between `{' and `}'"),
                 global_argv[argnum]);
        }
      p->repeat = val;
    }

  *end = '}';
}

