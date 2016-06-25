void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... FILE\n"), program_name);
      fputs (_("Print value of a symbolic link or canonical file name\n\n"),
             stdout);
      fputs (_("\
  -f, --canonicalize            canonicalize by following every symlink in\n\
                                every component of the given name recursively;\
\n\
                                all but the last component must exist\n\
  -e, --canonicalize-existing   canonicalize by following every symlink in\n\
                                every component of the given name recursively,\
\n\
                                all components must exist\n\
"), stdout);
      fputs (_("\
  -m, --canonicalize-missing    canonicalize by following every symlink in\n\
                                every component of the given name recursively,\
\n\
                                without requirements on components existence\n\
  -n, --no-newline              do not output the trailing newline\n\
  -q, --quiet,\n\
  -s, --silent                  suppress most error messages\n\
  -v, --verbose                 report error messages\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

