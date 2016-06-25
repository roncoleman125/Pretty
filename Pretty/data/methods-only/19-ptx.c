/* Option decoding and main program.  */

/*------------------------------------------------------.
| Print program identification and options, then exit.  |
`------------------------------------------------------*/

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [INPUT]...   (without -G)\n\
  or:  %s -G [OPTION]... [INPUT [OUTPUT]]\n"),
              program_name, program_name);
      fputs (_("\
Output a permuted index, including context, of the words in the input files.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -A, --auto-reference           output automatically generated references\n\
  -G, --traditional              behave more like System V `ptx'\n\
  -F, --flag-truncation=STRING   use STRING for flagging line truncations\n\
"), stdout);
      fputs (_("\
  -M, --macro-name=STRING        macro name to use instead of `xx'\n\
  -O, --format=roff              generate output as roff directives\n\
  -R, --right-side-refs          put references at right, not counted in -w\n\
  -S, --sentence-regexp=REGEXP   for end of lines or end of sentences\n\
  -T, --format=tex               generate output as TeX directives\n\
"), stdout);
      fputs (_("\
  -W, --word-regexp=REGEXP       use REGEXP to match each keyword\n\
  -b, --break-file=FILE          word break characters in this FILE\n\
  -f, --ignore-case              fold lower case to upper case for sorting\n\
  -g, --gap-size=NUMBER          gap size in columns between output fields\n\
  -i, --ignore-file=FILE         read ignore word list from FILE\n\
  -o, --only-file=FILE           read only word list from this FILE\n\
"), stdout);
      fputs (_("\
  -r, --references               first field of each line is a reference\n\
  -t, --typeset-mode               - not implemented -\n\
  -w, --width=NUMBER             output width in columns, reference excluded\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
With no FILE or if FILE is -, read Standard Input.  `-F /' by default.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

/*----------------------------------------------------------------------.
| Main program.  Decode ARGC arguments passed through the ARGV array of |
| strings, then launch execution.				        |
`----------------------------------------------------------------------*/

/* Long options equivalences.  */
static struct option const long_options[] =
{
  {"auto-reference", no_argument, NULL, 'A'},
  {"break-file", required_argument, NULL, 'b'},
  {"flag-truncation", required_argument, NULL, 'F'},
  {"ignore-case", no_argument, NULL, 'f'},
  {"gap-size", required_argument, NULL, 'g'},
  {"ignore-file", required_argument, NULL, 'i'},
  {"macro-name", required_argument, NULL, 'M'},
  {"only-file", required_argument, NULL, 'o'},
  {"references", no_argument, NULL, 'r'},
  {"right-side-refs", no_argument, NULL, 'R'},
  {"format", required_argument, NULL, 10},
  {"sentence-regexp", required_argument, NULL, 'S'},
  {"traditional", no_argument, NULL, 'G'},
  {"typeset-mode", no_argument, NULL, 't'},
  {"width", required_argument, NULL, 'w'},
  {"word-regexp", required_argument, NULL, 'W'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0},
};

static char const* const format_args[] =
{
  "roff", "tex", NULL
};

static enum Format const format_vals[] =
{
  ROFF_FORMAT, TEX_FORMAT
};

