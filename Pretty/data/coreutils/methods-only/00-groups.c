void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [USERNAME]...\n"), program_name);
      fputs (_("\
Print group memberships for each USERNAME or, if no USERNAME is specified, for\
\n\
the current process (which may differ if the groups database has changed).\n"),
             stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

