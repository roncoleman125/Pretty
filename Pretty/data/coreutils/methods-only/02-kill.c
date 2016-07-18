/* Print a list of signal names.  If TABLE, print a table.
   Print the names specified by ARGV if nonzero; otherwise,
   print all known names.  Return a suitable exit status.  */

static int
list_signals (bool table, char *const *argv)
{
  int signum;
  int status = EXIT_SUCCESS;
  char signame[SIG2STR_MAX];

  if (table)
    {
      unsigned int name_width = 0;

      /* Compute the maximum width of a signal number.  */
      unsigned int num_width = 1;
      for (signum = 1; signum <= SIGNUM_BOUND / 10; signum *= 10)
        num_width++;

      /* Compute the maximum width of a signal name.  */
      for (signum = 1; signum <= SIGNUM_BOUND; signum++)
        if (sig2str (signum, signame) == 0)
          {
            size_t len = strlen (signame);
            if (name_width < len)
              name_width = len;
          }

      if (argv)
        for (; *argv; argv++)
          {
            signum = operand2sig (*argv, signame);
            if (signum < 0)
              status = EXIT_FAILURE;
            else
              print_table_row (num_width, signum, name_width, signame);
          }
      else
        for (signum = 1; signum <= SIGNUM_BOUND; signum++)
          if (sig2str (signum, signame) == 0)
            print_table_row (num_width, signum, name_width, signame);
    }
  else
    {
      if (argv)
        for (; *argv; argv++)
          {
            signum = operand2sig (*argv, signame);
            if (signum < 0)
              status = EXIT_FAILURE;
            else
              {
                if (ISDIGIT (**argv))
                  puts (signame);
                else
                  printf ("%d\n", signum);
              }
          }
      else
        for (signum = 1; signum <= SIGNUM_BOUND; signum++)
          if (sig2str (signum, signame) == 0)
            puts (signame);
    }

  return status;
}

