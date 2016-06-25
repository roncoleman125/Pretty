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
Remove (unlink) the FILE(s).\n\
\n\
  -f, --force           ignore nonexistent files, never prompt\n\
  -i                    prompt before every removal\n\
"), stdout);
      fputs (_("\
  -I                    prompt once before removing more than three files, or\n\
                          when removing recursively.  Less intrusive than -i,\n\
                          while still giving protection against most mistakes\n\
      --interactive[=WHEN]  prompt according to WHEN: never, once (-I), or\n\
                          always (-i).  Without WHEN, prompt always\n\
"), stdout);
      fputs (_("\
      --one-file-system  when removing a hierarchy recursively, skip any\n\
                          directory that is on a file system different from\n\
                          that of the corresponding command line argument\n\
"), stdout);
      fputs (_("\
      --no-preserve-root  do not treat `/' specially\n\
      --preserve-root   do not remove `/' (default)\n\
  -r, -R, --recursive   remove directories and their contents recursively\n\
  -v, --verbose         explain what is being done\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
By default, rm does not remove directories.  Use the --recursive (-r or -R)\n\
option to remove each listed directory, too, along with all of its contents.\n\
"), stdout);
      printf (_("\
\n\
To remove a file whose name starts with a `-', for example `-foo',\n\
use one of these commands:\n\
  %s -- -foo\n\
\n\
  %s ./-foo\n\
"),
              program_name, program_name);
      fputs (_("\
\n\
Note that if you use rm to remove a file, it might be possible to recover\n\
some of its contents, given sufficient expertise and/or time.  For greater\n\
assurance that the contents are truly unrecoverable, consider using shred.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

