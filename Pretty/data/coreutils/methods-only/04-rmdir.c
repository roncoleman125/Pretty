void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... DIRECTORY...\n"), program_name);
      fputs (_("\
Remove the DIRECTORY(ies), if they are empty.\n\
\n\
      --ignore-fail-on-non-empty\n\
                  ignore each failure that is solely because a directory\n\
                    is non-empty\n\
"), stdout);
      fputs (_("\
  -p, --parents   remove DIRECTORY and its ancestors; e.g., `rmdir -p a/b/c' is\
\n\
                    similar to `rmdir a/b/c a/b a'\n\
  -v, --verbose   output a diagnostic for every directory processed\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

