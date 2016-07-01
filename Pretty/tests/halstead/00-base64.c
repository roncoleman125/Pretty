void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [FILE]\n\
Base64 encode or decode FILE, or standard input, to standard output.\n\
\n"), program_name);
      fputs (_("\
  -d, --decode          decode data\n\
  -i, --ignore-garbage  when decoding, ignore non-alphabet characters\n\
  -w, --wrap=COLS       wrap encoded lines after COLS character (default 76).\n\
                          Use 0 to disable line wrapping\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
With no FILE, or when FILE is -, read standard input.\n"), stdout);
      fputs (_("\
\n\
The data are encoded as described for the base64 alphabet in RFC 3548.\n\
When decoding, the input may contain newlines in addition to the bytes of\n\
the formal base64 alphabet.  Use --ignore-garbage to attempt to recover\n\
from any other non-alphabet bytes in the encoded stream.\n"),
             stdout);
      emit_ancillary_info ();
    }

  exit (status);
}

/* Note that increasing this may decrease performance if --ignore-garbage
   is used, because of the memmove operation below. */

/* Ensure that BLOCKSIZE is a multiple of 3 and 4.  */

