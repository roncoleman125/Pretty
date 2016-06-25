void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [USERNAME]\n"), program_name);
      fputs (_("\
Print user and group information for the specified USERNAME,\n\
or (when USERNAME omitted) for the current user.\n\
\n\
  -a              ignore, for compatibility with other versions\n\
  -Z, --context   print only the security context of the current user\n\
  -g, --group     print only the effective group ID\n\
  -G, --groups    print all group IDs\n\
  -n, --name      print a name instead of a number, for -ugG\n\
  -r, --real      print the real ID instead of the effective ID, with -ugG\n\
  -u, --user      print only the effective user ID\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Without any OPTION, print some useful set of identified information.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

