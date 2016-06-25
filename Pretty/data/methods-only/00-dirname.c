void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s NAME\n\
  or:  %s OPTION\n\
"),
              program_name, program_name);
      fputs (_("\
Output NAME with its last non-slash component and trailing slashes removed;\n\
if NAME contains no /'s, output `.' (meaning the current directory).\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\
\n\
Examples:\n\
  %s /usr/bin/      Output \"/usr\".\n\
  %s stdio.h        Output \".\".\n\
"),
              program_name, program_name);
      emit_ancillary_info ();
    }
  exit (status);
}

