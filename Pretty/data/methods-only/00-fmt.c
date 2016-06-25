void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [-WIDTH] [OPTION]... [FILE]...\n"), program_name);
      fputs (_("\
Reformat each paragraph in the FILE(s), writing to standard output.\n\
The option -WIDTH is an abbreviated form of --width=DIGITS.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -c, --crown-margin        preserve indentation of first two lines\n\
  -p, --prefix=STRING       reformat only lines beginning with STRING,\n\
                              reattaching the prefix to reformatted lines\n\
  -s, --split-only          split long lines, but do not refill\n\
"),
             stdout);
      fputs (_("\
  -t, --tagged-paragraph    indentation of first line different from second\n\
  -u, --uniform-spacing     one space between words, two after sentences\n\
  -w, --width=WIDTH         maximum line width (default of 75 columns)\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
With no FILE, or when FILE is -, read standard input.\n"),
             stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

/* Decode options and launch execution.  */

static struct option const long_options[] =
{
  {"crown-margin", no_argument, NULL, 'c'},
  {"prefix", required_argument, NULL, 'p'},
  {"split-only", no_argument, NULL, 's'},
  {"tagged-paragraph", no_argument, NULL, 't'},
  {"uniform-spacing", no_argument, NULL, 'u'},
  {"width", required_argument, NULL, 'w'},
  {GETOPT_HELP_OPTION_DECL},
  {GETOPT_VERSION_OPTION_DECL},
  {NULL, 0, NULL, 0},
};

