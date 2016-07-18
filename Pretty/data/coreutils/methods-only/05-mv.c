void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [-T] SOURCE DEST\n\
  or:  %s [OPTION]... SOURCE... DIRECTORY\n\
  or:  %s [OPTION]... -t DIRECTORY SOURCE...\n\
"),
              program_name, program_name, program_name);
      fputs (_("\
Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
      --backup[=CONTROL]       make a backup of each existing destination file\
\n\
  -b                           like --backup but does not accept an argument\n\
  -f, --force                  do not prompt before overwriting\n\
  -i, --interactive            prompt before overwrite\n\
  -n, --no-clobber             do not overwrite an existing file\n\
If you specify more than one of -i, -f, -n, only the final one takes effect.\n\
"), stdout);
      fputs (_("\
      --strip-trailing-slashes  remove any trailing slashes from each SOURCE\n\
                                 argument\n\
  -S, --suffix=SUFFIX          override the usual backup suffix\n\
"), stdout);
      fputs (_("\
  -t, --target-directory=DIRECTORY  move all SOURCE arguments into DIRECTORY\n\
  -T, --no-target-directory    treat DEST as a normal file\n\
  -u, --update                 move only when the SOURCE file is newer\n\
                                 than the destination file or when the\n\
                                 destination file is missing\n\
  -v, --verbose                explain what is being done\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
The backup suffix is `~', unless set with --suffix or SIMPLE_BACKUP_SUFFIX.\n\
The version control method may be selected via the --backup option or through\n\
the VERSION_CONTROL environment variable.  Here are the values:\n\
\n\
"), stdout);
      fputs (_("\
  none, off       never make backups (even if --backup is given)\n\
  numbered, t     make numbered backups\n\
  existing, nil   numbered if numbered backups exist, simple otherwise\n\
  simple, never   always make simple backups\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

