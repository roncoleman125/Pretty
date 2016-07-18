void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... FILE...\n"), program_name);
      fputs (_("\
Update the access and modification times of each FILE to the current time.\n\
\n\
A FILE argument that does not exist is created empty, unless -c or -h\n\
is supplied.\n\
\n\
A FILE argument string of - is handled specially and causes touch to\n\
change the times of the file associated with standard output.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -a                     change only the access time\n\
  -c, --no-create        do not create any files\n\
  -d, --date=STRING      parse STRING and use it instead of current time\n\
  -f                     (ignored)\n\
"), stdout);
      fputs (_("\
  -h, --no-dereference   affect each symbolic link instead of any referenced\n\
                         file (useful only on systems that can change the\n\
                         timestamps of a symlink)\n\
  -m                     change only the modification time\n\
"), stdout);
      fputs (_("\
  -r, --reference=FILE   use this file's times instead of current time\n\
  -t STAMP               use [[CC]YY]MMDDhhmm[.ss] instead of current time\n\
  --time=WORD            change the specified time:\n\
                           WORD is access, atime, or use: equivalent to -a\n\
                           WORD is modify or mtime: equivalent to -m\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Note that the -d and -t options accept different time-date formats.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

