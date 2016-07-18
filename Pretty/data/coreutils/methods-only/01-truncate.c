void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s OPTION... FILE...\n"), program_name);
      fputs (_("\
Shrink or extend the size of each FILE to the specified size\n\
\n\
A FILE argument that does not exist is created.\n\
\n\
If a FILE is larger than the specified size, the extra data is lost.\n\
If a FILE is shorter, it is extended and the extended part (hole)\n\
reads as zero bytes.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -c, --no-create        do not create any files\n\
"), stdout);
      fputs (_("\
  -o, --io-blocks        treat SIZE as number of IO blocks instead of bytes\n\
"), stdout);
      fputs (_("\
  -r, --reference=RFILE  base size on RFILE\n\
  -s, --size=SIZE        set or adjust the file size by SIZE\n"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_size_note ();
      fputs (_("\n\
SIZE may also be prefixed by one of the following modifying characters:\n\
`+' extend by, `-' reduce by, `<' at most, `>' at least,\n\
`/' round down to multiple of, `%' round up to multiple of.\n"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

