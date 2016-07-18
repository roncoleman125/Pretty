/* Advise the user about invalid usages like "rm -foo" if the file
   "-foo" exists, assuming ARGC and ARGV are as with `main'.  */

static void
diagnose_leading_hyphen (int argc, char **argv)
{
  /* OPTIND is unreliable, so iterate through the arguments looking
     for a file name that looks like an option.  */
  int i;

  for (i = 1; i < argc; i++)
    {
      char const *arg = argv[i];
      struct stat st;

      if (arg[0] == '-' && arg[1] && lstat (arg, &st) == 0)
        {
          fprintf (stderr,
                   _("Try `%s ./%s' to remove the file %s.\n"),
                   argv[0],
                   quotearg_n_style (1, shell_quoting_style, arg),
                   quote (arg));
          break;
        }
    }
}

