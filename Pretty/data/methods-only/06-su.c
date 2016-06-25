void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [-] [USER [ARG]...]\n"), program_name);
      fputs (_("\
Change the effective user id and group id to that of USER.\n\
\n\
  -, -l, --login               make the shell a login shell\n\
  -c, --command=COMMAND        pass a single COMMAND to the shell with -c\n\
  -f, --fast                   pass -f to the shell (for csh or tcsh)\n\
  -m, --preserve-environment   do not reset environment variables\n\
  -p                           same as -m\n\
  -s, --shell=SHELL            run SHELL if /etc/shells allows it\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
A mere - implies -l.   If USER not given, assume root.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

