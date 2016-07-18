void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [SHORT-OPTION]... [STRING]...\n\
  or:  %s LONG-OPTION\n\
"), program_name, program_name);
      fputs (_("\
Echo the STRING(s) to standard output.\n\
\n\
  -n             do not output the trailing newline\n\
"), stdout);
      fputs (_(DEFAULT_ECHO_TO_XPG
               ? N_("\
  -e             enable interpretation of backslash escapes (default)\n\
  -E             disable interpretation of backslash escapes\n")
               : N_("\
  -e             enable interpretation of backslash escapes\n\
  -E             disable interpretation of backslash escapes (default)\n")),
             stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
If -e is in effect, the following sequences are recognized:\n\
\n\
"), stdout);
      fputs (_("\
  \\\\      backslash\n\
  \\a      alert (BEL)\n\
  \\b      backspace\n\
  \\c      produce no further output\n\
  \\e      escape\n\
  \\f      form feed\n\
  \\n      new line\n\
  \\r      carriage return\n\
  \\t      horizontal tab\n\
  \\v      vertical tab\n\
"), stdout);
      fputs (_("\
  \\0NNN   byte with octal value NNN (1 to 3 digits)\n\
  \\xHH    byte with hexadecimal value HH (1 to 2 digits)\n\
"), stdout);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

