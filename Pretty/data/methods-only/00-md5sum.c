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
Print or check %s (%d-bit) checksums.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"),
              program_name,
              DIGEST_TYPE_STRING,
              DIGEST_BITS);
      if (O_BINARY)
        fputs (_("\
  -b, --binary         read in binary mode (default unless reading tty stdin)\n\
"), stdout);
      else
        fputs (_("\
  -b, --binary         read in binary mode\n\
"), stdout);
      printf (_("\
  -c, --check          read %s sums from the FILEs and check them\n"),
              DIGEST_TYPE_STRING);
      if (O_BINARY)
        fputs (_("\
  -t, --text           read in text mode (default if reading tty stdin)\n\
"), stdout);
      else
        fputs (_("\
  -t, --text           read in text mode (default)\n\
"), stdout);
      fputs (_("\
\n\
The following three options are useful only when verifying checksums:\n\
      --quiet          don't print OK for each successfully verified file\n\
      --status         don't output anything, status code shows success\n\
  -w, --warn           warn about improperly formatted checksum lines\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\
\n\
The sums are computed as described in %s.  When checking, the input\n\
should be a former output of this program.  The default mode is to print\n\
a line with checksum, a character indicating type (`*' for binary, ` ' for\n\
text), and name for each FILE.\n"),
              DIGEST_REFERENCE);
      emit_ancillary_info ();
    }

  exit (status);
}


