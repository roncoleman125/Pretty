void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [FILE]\n"), program_name);
      fputs (_("\
Output commands to set the LS_COLORS environment variable.\n\
\n\
Determine format of output:\n\
  -b, --sh, --bourne-shell    output Bourne shell code to set LS_COLORS\n\
  -c, --csh, --c-shell        output C shell code to set LS_COLORS\n\
  -p, --print-database        output defaults\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
If FILE is specified, read it to determine which colors to use for which\n\
file types and extensions.  Otherwise, a precompiled database is used.\n\
For details on the format of these files, run `dircolors --print-database'.\n\
"), stdout);
      emit_ancillary_info ();
    }

  exit (status);
}

