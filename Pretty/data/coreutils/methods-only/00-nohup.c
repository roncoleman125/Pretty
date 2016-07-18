void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s COMMAND [ARG]...\n\
  or:  %s OPTION\n\
"),
              program_name, program_name);

      fputs (_("\
Run COMMAND, ignoring hangup signals.\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\n\
If standard input is a terminal, redirect it from /dev/null.\n\
If standard output is a terminal, append output to `nohup.out' if possible,\n\
`$HOME/nohup.out' otherwise.\n\
If standard error is a terminal, redirect it to standard output.\n\
To save output to FILE, use `%s COMMAND > FILE'.\n"),
              program_name);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

