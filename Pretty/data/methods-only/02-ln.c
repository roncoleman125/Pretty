void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [-T] TARGET LINK_NAME   (1st form)\n\
  or:  %s [OPTION]... TARGET                  (2nd form)\n\
  or:  %s [OPTION]... TARGET... DIRECTORY     (3rd form)\n\
  or:  %s [OPTION]... -t DIRECTORY TARGET...  (4th form)\n\
"),
              program_name, program_name, program_name, program_name);
      fputs (_("\
In the 1st form, create a link to TARGET with the name LINK_NAME.\n\
In the 2nd form, create a link to TARGET in the current directory.\n\
In the 3rd and 4th forms, create links to each TARGET in DIRECTORY.\n\
Create hard links by default, symbolic links with --symbolic.\n\
When creating hard links, each TARGET must exist.  Symbolic links\n\
can hold arbitrary text; if later resolved, a relative link is\n\
interpreted in relation to its parent directory.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
      --backup[=CONTROL]      make a backup of each existing destination file\n\
  -b                          like --backup but does not accept an argument\n\
  -d, -F, --directory         allow the superuser to attempt to hard link\n\
                                directories (note: will probably fail due to\n\
                                system restrictions, even for the superuser)\n\
  -f, --force                 remove existing destination files\n\
"), stdout);
      fputs (_("\
  -i, --interactive           prompt whether to remove destinations\n\
  -L, --logical               make hard links to symbolic link references\n\
  -n, --no-dereference        treat destination that is a symlink to a\n\
                                directory as if it were a normal file\n\
  -P, --physical              make hard links directly to symbolic links\n\
  -s, --symbolic              make symbolic links instead of hard links\n\
"), stdout);
      fputs (_("\
  -S, --suffix=SUFFIX         override the usual backup suffix\n\
  -t, --target-directory=DIRECTORY  specify the DIRECTORY in which to create\n\
                                the links\n\
  -T, --no-target-directory   treat LINK_NAME as a normal file\n\
  -v, --verbose               print name of each linked file\n\
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
      printf (_("\
Using -s ignores -L and -P.  Otherwise, the last option specified controls\n\
behavior when the source is a symbolic link, defaulting to %s.\n\
\n\
"), LINK_FOLLOWS_SYMLINKS ? "-L" : "-P");
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

