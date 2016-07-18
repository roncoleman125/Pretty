void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [-] [NAME=VALUE]... [COMMAND [ARG]...]\n"),
              program_name);
      fputs (_("\
Set each NAME to VALUE in the environment and run COMMAND.\n\
\n\
  -i, --ignore-environment  start with an empty environment\n\
  -0, --null           end each output line with 0 byte rather than newline\n\
  -u, --unset=NAME     remove variable from the environment\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
A mere - implies -i.  If no COMMAND, print the resulting environment.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

