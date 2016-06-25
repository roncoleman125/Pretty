/* Complain, print a usage message, and die. */

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
Paginate or columnate FILE(s) for printing.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  +FIRST_PAGE[:LAST_PAGE], --pages=FIRST_PAGE[:LAST_PAGE]\n\
                    begin [stop] printing with page FIRST_[LAST_]PAGE\n\
  -COLUMN, --columns=COLUMN\n\
                    output COLUMN columns and print columns down,\n\
                    unless -a is used. Balance number of lines in the\n\
                    columns on each page\n\
"), stdout);
      fputs (_("\
  -a, --across      print columns across rather than down, used together\n\
                    with -COLUMN\n\
  -c, --show-control-chars\n\
                    use hat notation (^G) and octal backslash notation\n\
  -d, --double-space\n\
                    double space the output\n\
"), stdout);
      fputs (_("\
  -D, --date-format=FORMAT\n\
                    use FORMAT for the header date\n\
  -e[CHAR[WIDTH]], --expand-tabs[=CHAR[WIDTH]]\n\
                    expand input CHARs (TABs) to tab WIDTH (8)\n\
  -F, -f, --form-feed\n\
                    use form feeds instead of newlines to separate pages\n\
                    (by a 3-line page header with -F or a 5-line header\n\
                    and trailer without -F)\n\
"), stdout);
      fputs (_("\
  -h, --header=HEADER\n\
                    use a centered HEADER instead of filename in page header,\n\
                    -h \"\" prints a blank line, don't use -h\"\"\n\
  -i[CHAR[WIDTH]], --output-tabs[=CHAR[WIDTH]]\n\
                    replace spaces with CHARs (TABs) to tab WIDTH (8)\n\
  -J, --join-lines  merge full lines, turns off -W line truncation, no column\n\
                    alignment, --sep-string[=STRING] sets separators\n\
"), stdout);
      fputs (_("\
  -l, --length=PAGE_LENGTH\n\
                    set the page length to PAGE_LENGTH (66) lines\n\
                    (default number of lines of text 56, and with -F 63)\n\
  -m, --merge       print all files in parallel, one in each column,\n\
                    truncate lines, but join lines of full length with -J\n\
"), stdout);
      fputs (_("\
  -n[SEP[DIGITS]], --number-lines[=SEP[DIGITS]]\n\
                    number lines, use DIGITS (5) digits, then SEP (TAB),\n\
                    default counting starts with 1st line of input file\n\
  -N, --first-line-number=NUMBER\n\
                    start counting with NUMBER at 1st line of first\n\
                    page printed (see +FIRST_PAGE)\n\
"), stdout);
      fputs (_("\
  -o, --indent=MARGIN\n\
                    offset each line with MARGIN (zero) spaces, do not\n\
                    affect -w or -W, MARGIN will be added to PAGE_WIDTH\n\
  -r, --no-file-warnings\n\
                    omit warning when a file cannot be opened\n\
"), stdout);
      fputs (_("\
  -s[CHAR], --separator[=CHAR]\n\
                    separate columns by a single character, default for CHAR\n\
                    is the <TAB> character without -w and \'no char\' with -w\n\
                    -s[CHAR] turns off line truncation of all 3 column\n\
                    options (-COLUMN|-a -COLUMN|-m) except -w is set\n\
"), stdout);
      fputs (_("\
  -SSTRING, --sep-string[=STRING]\n\
                    separate columns by STRING,\n\
                    without -S: Default separator <TAB> with -J and <space>\n\
                    otherwise (same as -S\" \"), no effect on column options\n\
  -t, --omit-header  omit page headers and trailers\n\
"), stdout);
      fputs (_("\
  -T, --omit-pagination\n\
                    omit page headers and trailers, eliminate any pagination\n\
                    by form feeds set in input files\n\
  -v, --show-nonprinting\n\
                    use octal backslash notation\n\
  -w, --width=PAGE_WIDTH\n\
                    set page width to PAGE_WIDTH (72) characters for\n\
                    multiple text-column output only, -s[char] turns off (72)\n\
"), stdout);
      fputs (_("\
  -W, --page-width=PAGE_WIDTH\n\
                    set page width to PAGE_WIDTH (72) characters always,\n\
                    truncate lines, except -J option is set, no interference\n\
                    with -S or -s\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
-t is implied if PAGE_LENGTH <= 10.  With no FILE, or when FILE is -, read\n\
standard input.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}
