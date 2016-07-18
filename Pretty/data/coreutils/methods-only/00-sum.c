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
Print checksum and block counts for each FILE.\n\
\n\
  -r              use BSD sum algorithm, use 1K blocks\n\
  -s, --sysv      use System V sum algorithm, use 512 bytes blocks\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
With no FILE, or when FILE is -, read standard input.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

