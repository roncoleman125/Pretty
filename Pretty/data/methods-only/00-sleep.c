void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s NUMBER[SUFFIX]...\n\
  or:  %s OPTION\n\
Pause for NUMBER seconds.  SUFFIX may be `s' for seconds (the default),\n\
`m' for minutes, `h' for hours or `d' for days.  Unlike most implementations\n\
that require NUMBER be an integer, here NUMBER may be an arbitrary floating\n\
point number.  Given two or more arguments, pause for the amount of time\n\
specified by the sum of their values.\n\
\n\
"),
              program_name, program_name);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

