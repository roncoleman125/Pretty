void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... MODE[,MODE]... FILE...\n\
  or:  %s [OPTION]... OCTAL-MODE FILE...\n\
  or:  %s [OPTION]... --reference=RFILE FILE...\n\
"),
              program_name, program_name, program_name);
      fputs (_("\
Change the mode of each FILE to MODE.\n\
\n\
  -c, --changes           like verbose but report only when a change is made\n\
"), stdout);
      fputs (_("\
      --no-preserve-root  do not treat `/' specially (the default)\n\
      --preserve-root     fail to operate recursively on `/'\n\
"), stdout);
      fputs (_("\
  -f, --silent, --quiet   suppress most error messages\n\
  -v, --verbose           output a diagnostic for every file processed\n\
      --reference=RFILE   use RFILE's mode instead of MODE values\n\
  -R, --recursive         change files and directories recursively\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Each MODE is of the form `[ugoa]*([-+=]([rwxXst]*|[ugo]))+'.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

