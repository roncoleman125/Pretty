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
Create the DIRECTORY(ies), if they do not already exist.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -m, --mode=MODE   set file mode (as in chmod), not a=rwx - umask\n\
  -p, --parents     no error if existing, make parent directories as needed\n\
  -v, --verbose     print a message for each created directory\n\
  -Z, --context=CTX  set the SELinux security context of each created\n\
                      directory to CTX\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

/* Options passed to subsidiary functions.  */
struct mkdir_options
{
  /* Function to make an ancestor, or NULL if ancestors should not be
     made.  */
  int (*make_ancestor_function) (char const *, char const *, void *);

  /* Mode for ancestor directory.  */
  mode_t ancestor_mode;

  /* Mode for directory itself.  */
  mode_t mode;

  /* File mode bits affected by MODE.  */
  mode_t mode_bits;

  /* If not null, format to use when reporting newly made directories.  */
  char const *created_directory_format;
};

