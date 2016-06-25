/* Extract the break patterns from args START through ARGC - 1 of ARGV.
   After each pattern, check if the next argument is a repeat count. */

static void
parse_patterns (int argc, int start, char **argv)
{
  int i;			/* Index into ARGV. */
  struct control *p;		/* New control record created. */
  uintmax_t val;
  static uintmax_t last_val = 0;

  for (i = start; i < argc; i++)
    {
      if (*argv[i] == '/' || *argv[i] == '%')
        {
          p = extract_regexp (i, *argv[i] == '%', argv[i]);
        }
      else
        {
          p = new_control_record ();
          p->argnum = i;

          if (xstrtoumax (argv[i], NULL, 10, &val, "") != LONGINT_OK)
            error (EXIT_FAILURE, 0, _("%s: invalid pattern"), argv[i]);
          if (val == 0)
            error (EXIT_FAILURE, 0,
                   _("%s: line number must be greater than zero"),
                   argv[i]);
          if (val < last_val)
            {
              char buf[INT_BUFSIZE_BOUND (uintmax_t)];
              error (EXIT_FAILURE, 0,
               _("line number %s is smaller than preceding line number, %s"),
                     quote (argv[i]), umaxtostr (last_val, buf));
            }

          if (val == last_val)
            error (0, 0,
           _("warning: line number %s is the same as preceding line number"),
                   quote (argv[i]));

          last_val = val;

          p->lines_required = val;
        }

      if (i + 1 < argc && *argv[i + 1] == '{')
        {
          /* We have a repeat count. */
          i++;
          parse_repeat_count (i, p, argv[i]);
        }
    }
}



/* Names for the printf format flags ' and #.  These can be ORed together.  */
enum { FLAG_THOUSANDS = 1, FLAG_ALTERNATIVE = 2 };

