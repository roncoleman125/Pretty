void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [TEMPLATE]\n"), program_name);
      fputs (_("\
Create a temporary file or directory, safely, and print its name.\n\
TEMPLATE must contain at least 3 consecutive `X's in last component.\n\
If TEMPLATE is not specified, use tmp.XXXXXXXXXX, and --tmpdir is implied.\n\
"), stdout);
      fputs ("\n", stdout);
      fputs (_("\
  -d, --directory     create a directory, not a file\n\
  -u, --dry-run       do not create anything; merely print a name (unsafe)\n\
  -q, --quiet         suppress diagnostics about file/dir-creation failure\n\
"), stdout);
      fputs (_("\
      --suffix=SUFF   append SUFF to TEMPLATE.  SUFF must not contain slash.\n\
                        This option is implied if TEMPLATE does not end in X.\n\
"), stdout);
      fputs (_("\
      --tmpdir[=DIR]  interpret TEMPLATE relative to DIR.  If DIR is not\n\
                        specified, use $TMPDIR if set, else /tmp.  With\n\
                        this option, TEMPLATE must not be an absolute name.\n\
                        Unlike with -t, TEMPLATE may contain slashes, but\n\
                        mktemp creates only the final component\n\
"), stdout);
      fputs ("\n", stdout);
      fputs (_("\
  -p DIR              use DIR as a prefix; implies -t [deprecated]\n\
  -t                  interpret TEMPLATE as a single file name component,\n\
                        relative to a directory: $TMPDIR, if set; else the\n\
                        directory specified via -p; else /tmp [deprecated]\n\
"), stdout);
      fputs ("\n", stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }

  exit (status);
}

