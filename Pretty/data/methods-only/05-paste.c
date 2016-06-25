void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [FILE]...\n\
"),
              program_name);
      fputs (_("\
Write lines consisting of the sequentially corresponding lines from\n\
each FILE, separated by TABs, to standard output.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -d, --delimiters=LIST   reuse characters from LIST instead of TABs\n\
  -s, --serial            paste one file at a time instead of in parallel\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      /* FIXME: add a couple of examples.  */
      emit_ancillary_info ();
    }
  exit (status);
}

