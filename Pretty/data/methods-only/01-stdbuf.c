void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s OPTION... COMMAND\n"), program_name);
      fputs (_("\
Run COMMAND, with modified buffering operations for its standard streams.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -i, --input=MODE   adjust standard input stream buffering\n\
  -o, --output=MODE  adjust standard output stream buffering\n\
  -e, --error=MODE   adjust standard error stream buffering\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\n\
If MODE is `L' the corresponding stream will be line buffered.\n\
This option is invalid with standard input.\n"), stdout);
      fputs (_("\n\
If MODE is `0' the corresponding stream will be unbuffered.\n\
"), stdout);
      fputs (_("\n\
Otherwise MODE is a number which may be followed by one of the following:\n\
KB 1000, K 1024, MB 1000*1000, M 1024*1024, and so on for G, T, P, E, Z, Y.\n\
In this case the corresponding stream will be fully buffered with the buffer\n\
size set to MODE bytes.\n\
"), stdout);
      fputs (_("\n\
NOTE: If COMMAND adjusts the buffering of its standard streams (`tee' does\n\
for e.g.) then that will override corresponding settings changed by `stdbuf'.\n\
Also some filters (like `dd' and `cat' etc.) don't use streams for I/O,\n\
and are thus unaffected by `stdbuf' settings.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

