void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... SET1 [SET2]\n\
"),
              program_name);
      fputs (_("\
Translate, squeeze, and/or delete characters from standard input,\n\
writing to standard output.\n\
\n\
  -c, -C, --complement    use the complement of SET1\n\
  -d, --delete            delete characters in SET1, do not translate\n\
  -s, --squeeze-repeats   replace each input sequence of a repeated character\n\
                            that is listed in SET1 with a single occurrence\n\
                            of that character\n\
  -t, --truncate-set1     first truncate SET1 to length of SET2\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
SETs are specified as strings of characters.  Most represent themselves.\n\
Interpreted sequences are:\n\
\n\
  \\NNN            character with octal value NNN (1 to 3 octal digits)\n\
  \\\\              backslash\n\
  \\a              audible BEL\n\
  \\b              backspace\n\
  \\f              form feed\n\
  \\n              new line\n\
  \\r              return\n\
  \\t              horizontal tab\n\
"), stdout);
     fputs (_("\
  \\v              vertical tab\n\
  CHAR1-CHAR2     all characters from CHAR1 to CHAR2 in ascending order\n\
  [CHAR*]         in SET2, copies of CHAR until length of SET1\n\
  [CHAR*REPEAT]   REPEAT copies of CHAR, REPEAT octal if starting with 0\n\
  [:alnum:]       all letters and digits\n\
  [:alpha:]       all letters\n\
  [:blank:]       all horizontal whitespace\n\
  [:cntrl:]       all control characters\n\
  [:digit:]       all digits\n\
"), stdout);
     fputs (_("\
  [:graph:]       all printable characters, not including space\n\
  [:lower:]       all lower case letters\n\
  [:print:]       all printable characters, including space\n\
  [:punct:]       all punctuation characters\n\
  [:space:]       all horizontal or vertical whitespace\n\
  [:upper:]       all upper case letters\n\
  [:xdigit:]      all hexadecimal digits\n\
  [=CHAR=]        all characters which are equivalent to CHAR\n\
"), stdout);
     fputs (_("\
\n\
Translation occurs if -d is not given and both SET1 and SET2 appear.\n\
-t may be used only when translating.  SET2 is extended to length of\n\
SET1 by repeating its last character as necessary.  Excess characters\n\
of SET2 are ignored.  Only [:lower:] and [:upper:] are guaranteed to\n\
expand in ascending order; used in SET2 while translating, they may\n\
only be used in pairs to specify case conversion.  -s uses SET1 if not\n\
translating nor deleting; else squeezing uses SET2 and occurs after\n\
translation or deletion.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

