void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION] DURATION COMMAND [ARG]...\n\
  or:  %s [OPTION]\n"), program_name, program_name);

      fputs (_("\
Start COMMAND, and kill it if still running after DURATION.\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -k, --kill-after=DURATION\n\
                   also send a KILL signal if COMMAND is still running\n\
                   this long after the initial signal was sent.\n\
  -s, --signal=SIGNAL\n\
                   specify the signal to be sent on timeout.\n\
                   SIGNAL may be a name like `HUP' or a number.\n\
                   See `kill -l` for a list of signals\n"), stdout);

      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);

      fputs (_("\n\
DURATION is an integer with an optional suffix:\n\
`s' for seconds(the default), `m' for minutes, `h' for hours or `d' for days.\n\
"), stdout);

      fputs (_("\n\
If the command times out, then exit with status 124.  Otherwise, exit\n\
with the status of COMMAND.  If no signal is specified, send the TERM\n\
signal upon timeout.  The TERM signal kills any process that does not\n\
block or catch that signal.  For other processes, it may be necessary to\n\
use the KILL (9) signal, since this signal cannot be caught.\n"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

