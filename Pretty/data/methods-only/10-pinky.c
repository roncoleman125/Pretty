void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [USER]...\n"), program_name);
      fputs (_("\
\n\
  -l              produce long format output for the specified USERs\n\
  -b              omit the user's home directory and shell in long format\n\
  -h              omit the user's project file in long format\n\
  -p              omit the user's plan file in long format\n\
  -s              do short format output, this is the default\n\
"), stdout);
      fputs (_("\
  -f              omit the line of column headings in short format\n\
  -w              omit the user's full name in short format\n\
  -i              omit the user's full name and remote host in short format\n\
  -q              omit the user's full name, remote host and idle time\n\
                  in short format\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\
\n\
A lightweight `finger' program;  print user information.\n\
The utmp file will be %s.\n\
"), UTMP_FILE);
      emit_ancillary_info ();
    }
  exit (status);
}

