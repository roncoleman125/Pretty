/* Print a usage message and quit. */

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
"),
              program_name);
      fputs (_("\
Write each FILE to standard output, with line numbers added.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -b, --body-numbering=STYLE      use STYLE for numbering body lines\n\
  -d, --section-delimiter=CC      use CC for separating logical pages\n\
  -f, --footer-numbering=STYLE    use STYLE for numbering footer lines\n\
"), stdout);
      fputs (_("\
  -h, --header-numbering=STYLE    use STYLE for numbering header lines\n\
  -i, --line-increment=NUMBER     line number increment at each line\n\
  -l, --join-blank-lines=NUMBER   group of NUMBER empty lines counted as one\n\
  -n, --number-format=FORMAT      insert line numbers according to FORMAT\n\
  -p, --no-renumber               do not reset line numbers at logical pages\n\
  -s, --number-separator=STRING   add STRING after (possible) line number\n\
"), stdout);
      fputs (_("\
  -v, --starting-line-number=NUMBER  first line number on each logical page\n\
  -w, --number-width=NUMBER       use NUMBER columns for line numbers\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
By default, selects -v1 -i1 -l1 -sTAB -w6 -nrn -hn -bt -fn.  CC are\n\
two delimiter characters for separating logical pages, a missing\n\
second character implies :.  Type \\\\ for \\.  STYLE is one of:\n\
"), stdout);
      fputs (_("\
\n\
  a         number all lines\n\
  t         number only nonempty lines\n\
  n         number no lines\n\
  pBRE      number only lines that contain a match for the basic regular\n\
            expression, BRE\n\
\n\
FORMAT is one of:\n\
\n\
  ln   left justified, no leading zeros\n\
  rn   right justified, no leading zeros\n\
  rz   right justified, leading zeros\n\
\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

