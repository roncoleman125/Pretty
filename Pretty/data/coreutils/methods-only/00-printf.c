void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s FORMAT [ARGUMENT]...\n\
  or:  %s OPTION\n\
"),
              program_name, program_name);
      fputs (_("\
Print ARGUMENT(s) according to FORMAT, or execute according to OPTION:\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
FORMAT controls the output as in C printf.  Interpreted sequences are:\n\
\n\
  \\\"      double quote\n\
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
  \\NNN    byte with octal value NNN (1 to 3 digits)\n\
  \\xHH    byte with hexadecimal value HH (1 to 2 digits)\n\
  \\uHHHH  Unicode (ISO/IEC 10646) character with hex value HHHH (4 digits)\n\
  \\UHHHHHHHH  Unicode character with hex value HHHHHHHH (8 digits)\n\
"), stdout);
      fputs (_("\
  %%      a single %\n\
  %b      ARGUMENT as a string with `\\' escapes interpreted,\n\
          except that octal escapes are of the form \\0 or \\0NNN\n\
\n\
and all C format specifications ending with one of diouxXfeEgGcs, with\n\
ARGUMENTs converted to proper type first.  Variable widths are handled.\n\
"), stdout);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

