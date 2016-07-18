void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s CONTEXT COMMAND [args]\n\
  or:  %s [ -c ] [-u USER] [-r ROLE] [-t TYPE] [-l RANGE] COMMAND [args]\n\
"), program_name, program_name);
      fputs (_("\
Run a program in a different security context.\n\
With neither CONTEXT nor COMMAND, print the current security context.\n\
\n\
  CONTEXT            Complete security context\n\
  -c, --compute      compute process transition context before modifying\n\
  -t, --type=TYPE    type (for same role as parent)\n\
  -u, --user=USER    user identity\n\
  -r, --role=ROLE    role\n\
  -l, --range=RANGE  levelrange\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

