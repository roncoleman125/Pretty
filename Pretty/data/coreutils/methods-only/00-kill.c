void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [-s SIGNAL | -SIGNAL] PID...\n\
  or:  %s -l [SIGNAL]...\n\
  or:  %s -t [SIGNAL]...\n\
"),
              program_name, program_name, program_name);
      fputs (_("\
Send signals to processes, or list signals.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -s, --signal=SIGNAL, -SIGNAL\n\
                   specify the name or number of the signal to be sent\n\
  -l, --list       list signal names, or convert signal names to/from numbers\n\
  -t, --table      print a table of signal information\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\n\
SIGNAL may be a signal name like `HUP', or a signal number like `1',\n\
or the exit status of a process terminated by a signal.\n\
PID is an integer; if negative it identifies a process group.\n\
"), stdout);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

