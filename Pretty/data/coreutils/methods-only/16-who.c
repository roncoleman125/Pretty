void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [ FILE | ARG1 ARG2 ]\n"), program_name);
      fputs (_("\
Print information about users who are currently logged in.\n\
"), stdout);
      fputs (_("\
\n\
  -a, --all         same as -b -d --login -p -r -t -T -u\n\
  -b, --boot        time of last system boot\n\
  -d, --dead        print dead processes\n\
  -H, --heading     print line of column headings\n\
"), stdout);
      fputs (_("\
  -l, --login       print system login processes\n\
"), stdout);
      fputs (_("\
      --lookup      attempt to canonicalize hostnames via DNS\n\
  -m                only hostname and user associated with stdin\n\
  -p, --process     print active processes spawned by init\n\
"), stdout);
      fputs (_("\
  -q, --count       all login names and number of users logged on\n\
  -r, --runlevel    print current runlevel\n\
  -s, --short       print only name, line, and time (default)\n\
  -t, --time        print last system clock change\n\
"), stdout);
      fputs (_("\
  -T, -w, --mesg    add user's message status as +, - or ?\n\
  -u, --users       list users logged in\n\
      --message     same as -T\n\
      --writable    same as -T\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\
\n\
If FILE is not specified, use %s.  %s as FILE is common.\n\
If ARG1 ARG2 given, -m presumed: `am i' or `mom likes' are usual.\n\
"), UTMP_FILE, WTMP_FILE);
      emit_ancillary_info ();
    }
  exit (status);
}

