void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [FILE]...\n\
  or:  %s [OPTION]... --files0-from=F\n\
"),
              program_name, program_name);
      fputs (_("\
Write sorted concatenation of all FILE(s) to standard output.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
Ordering options:\n\
\n\
"), stdout);
      fputs (_("\
  -b, --ignore-leading-blanks  ignore leading blanks\n\
  -d, --dictionary-order      consider only blanks and alphanumeric characters\
\n\
  -f, --ignore-case           fold lower case to upper case characters\n\
"), stdout);
      fputs (_("\
  -g, --general-numeric-sort  compare according to general numerical value\n\
  -i, --ignore-nonprinting    consider only printable characters\n\
  -M, --month-sort            compare (unknown) < `JAN' < ... < `DEC'\n\
"), stdout);
      fputs (_("\
  -h, --human-numeric-sort    compare human readable numbers (e.g., 2K 1G)\n\
"), stdout);
      fputs (_("\
  -n, --numeric-sort          compare according to string numerical value\n\
  -R, --random-sort           sort by random hash of keys\n\
      --random-source=FILE    get random bytes from FILE\n\
  -r, --reverse               reverse the result of comparisons\n\
"), stdout);
      fputs (_("\
      --sort=WORD             sort according to WORD:\n\
                                general-numeric -g, human-numeric -h, month -M,\
\n\
                                numeric -n, random -R, version -V\n\
  -V, --version-sort          natural sort of (version) numbers within text\n\
\n\
"), stdout);
      fputs (_("\
Other options:\n\
\n\
"), stdout);
      fputs (_("\
      --batch-size=NMERGE   merge at most NMERGE inputs at once;\n\
                            for more use temp files\n\
"), stdout);
      fputs (_("\
  -c, --check, --check=diagnose-first  check for sorted input; do not sort\n\
  -C, --check=quiet, --check=silent  like -c, but do not report first bad line\
\n\
      --compress-program=PROG  compress temporaries with PROG;\n\
                              decompress them with PROG -d\n\
"), stdout);
      fputs (_("\
      --debug               annotate the part of the line used to sort,\n\
                              and warn about questionable usage to stderr\n\
      --files0-from=F       read input from the files specified by\n\
                            NUL-terminated names in file F;\n\
                            If F is - then read names from standard input\n\
"), stdout);
      fputs (_("\
  -k, --key=POS1[,POS2]     start a key at POS1 (origin 1), end it at POS2\n\
                            (default end of line).  See POS syntax below\n\
  -m, --merge               merge already sorted files; do not sort\n\
"), stdout);
      fputs (_("\
  -o, --output=FILE         write result to FILE instead of standard output\n\
  -s, --stable              stabilize sort by disabling last-resort comparison\
\n\
  -S, --buffer-size=SIZE    use SIZE for main memory buffer\n\
"), stdout);
      printf (_("\
  -t, --field-separator=SEP  use SEP instead of non-blank to blank transition\n\
  -T, --temporary-directory=DIR  use DIR for temporaries, not $TMPDIR or %s;\n\
                              multiple options specify multiple directories\n\
      --parallel=N          change the number of sorts run concurrently to N\n\
  -u, --unique              with -c, check for strict ordering;\n\
                              without -c, output only the first of an equal run\
\n\
"), DEFAULT_TMPDIR);
      fputs (_("\
  -z, --zero-terminated     end lines with 0 byte, not newline\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
POS is F[.C][OPTS], where F is the field number and C the character position\n\
in the field; both are origin 1.  If neither -t nor -b is in effect, characters\
\n\
in a field are counted from the beginning of the preceding whitespace.  OPTS is\
\n\
one or more single-letter ordering options, which override global ordering\n\
options for that key.  If no key is given, use the entire line as the key.\n\
\n\
SIZE may be followed by the following multiplicative suffixes:\n\
"), stdout);
      fputs (_("\
% 1% of memory, b 1, K 1024 (default), and so on for M, G, T, P, E, Z, Y.\n\
\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
*** WARNING ***\n\
The locale specified by the environment affects sort order.\n\
Set LC_ALL=C to get the traditional sort order that uses\n\
native byte values.\n\
"), stdout );
      emit_ancillary_info ();
    }

  exit (status);
}

