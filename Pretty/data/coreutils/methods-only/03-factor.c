void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [NUMBER]...\n\
  or:  %s OPTION\n\
"),
              program_name, program_name);
      fputs (_("\
Print the prime factors of each specified integer NUMBER.  If none\n\
are specified on the command line, read them from standard input.\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

