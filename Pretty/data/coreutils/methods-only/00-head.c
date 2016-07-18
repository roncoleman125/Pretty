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
Print the first 10 lines of each FILE to standard output.\n\
With more than one FILE, precede each with a header giving the file name.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -c, --bytes=[-]K         print the first K bytes of each file;\n\
                             with the leading `-', print all but the last\n\
                             K bytes of each file\n\
  -n, --lines=[-]K         print the first K lines instead of the first 10;\n\
                             with the leading `-', print all but the last\n\
                             K lines of each file\n\
"), stdout);
      fputs (_("\
  -q, --quiet, --silent    never print headers giving file names\n\
  -v, --verbose            always print headers giving file names\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
K may have a multiplier suffix:\n\
b 512, kB 1000, K 1024, MB 1000*1000, M 1024*1024,\n\
GB 1000*1000*1000, G 1024*1024*1024, and so on for T, P, E, Z, Y.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

