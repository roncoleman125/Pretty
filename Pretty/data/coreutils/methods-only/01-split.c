void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [INPUT [PREFIX]]\n\
"),
              program_name);
    fputs (_("\
Output fixed-size pieces of INPUT to PREFIXaa, PREFIXab, ...; default\n\
size is 1000 lines, and default PREFIX is `x'.  With no INPUT, or when INPUT\n\
is -, read standard input.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fprintf (stdout, _("\
  -a, --suffix-length=N   use suffixes of length N (default %d)\n\
  -b, --bytes=SIZE        put SIZE bytes per output file\n\
  -C, --line-bytes=SIZE   put at most SIZE bytes of lines per output file\n\
  -d, --numeric-suffixes  use numeric suffixes instead of alphabetic\n\
  -e, --elide-empty-files  do not generate empty output files with `-n'\n\
  -l, --lines=NUMBER      put NUMBER lines per output file\n\
  -n, --number=CHUNKS     generate CHUNKS output files.  See below\n\
  -u, --unbuffered        immediately copy input to output with `-n r/...'\n\
"), DEFAULT_SUFFIX_LENGTH);
      fputs (_("\
      --verbose           print a diagnostic just before each\n\
                            output file is opened\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_size_note ();
fputs (_("\n\
CHUNKS may be:\n\
N       split into N files based on size of input\n\
K/N     output Kth of N to stdout\n\
l/N     split into N files without splitting lines\n\
l/K/N   output Kth of N to stdout without splitting lines\n\
r/N     like `l' but use round robin distribution\n\
r/K/N   likewise but only output Kth of N to stdout\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

