void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... NAME...\n"), program_name);
      fputs (_("\
Diagnose invalid or unportable file names.\n\
\n\
  -p                  check for most POSIX systems\n\
  -P                  check for empty names and leading \"-\"\n\
      --portability   check for all POSIX systems (equivalent to -p -P)\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

