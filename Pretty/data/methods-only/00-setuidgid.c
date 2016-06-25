void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [SHORT-OPTION]... USER COMMAND [ARGUMENT]...\n\
  or:  %s LONG-OPTION\n\
"),
              program_name, program_name);

      fputs (_("\
Drop any supplemental groups, assume the user-ID and group-ID of the specified\
\n\
USER (numeric ID or user name), and run COMMAND with any specified ARGUMENTs.\n\
Exit with status 111 if unable to assume the required user and group ID.\n\
Otherwise, exit with the exit status of COMMAND.\n\
This program is useful only when run by root (user ID zero).\n\
\n\
"), stdout);
      fputs (_("\
  -g GID[,GID1...]  also set the primary group-ID to the numeric GID, and\n\
                    (if specified) supplemental group IDs to GID1, ...\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

