void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [VARIABLE]...\n\
Print the values of the specified environment VARIABLE(s).\n\
If no VARIABLE is specified, print name and value pairs for them all.\n\
\n\
"),
              program_name);
      fputs (_("\
  -0, --null     end each output line with 0 byte rather than newline\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

