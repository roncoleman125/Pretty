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
Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -a, --archive                same as -dR --preserve=all\n\
      --attributes-only        don't copy the file data, just the attributes\n\
      --backup[=CONTROL]       make a backup of each existing destination file\
\n\
  -b                           like --backup but does not accept an argument\n\
      --copy-contents          copy contents of special files when recursive\n\
  -d                           same as --no-dereference --preserve=links\n\
"), stdout);
      fputs (_("\
  -f, --force                  if an existing destination file cannot be\n\
                                 opened, remove it and try again (redundant if\
\n\
                                 the -n option is used)\n\
  -i, --interactive            prompt before overwrite (overrides a previous -n\
\n\
                                  option)\n\
  -H                           follow command-line symbolic links in SOURCE\n\
"), stdout);
      fputs (_("\
  -l, --link                   link files instead of copying\n\
  -L, --dereference            always follow symbolic links in SOURCE\n\
"), stdout);
      fputs (_("\
  -n, --no-clobber             do not overwrite an existing file (overrides\n\
                                 a previous -i option)\n\
  -P, --no-dereference         never follow symbolic links in SOURCE\n\
"), stdout);
      fputs (_("\
  -p                           same as --preserve=mode,ownership,timestamps\n\
      --preserve[=ATTR_LIST]   preserve the specified attributes (default:\n\
                                 mode,ownership,timestamps), if possible\n\
                                 additional attributes: context, links, xattr,\
\n\
                                 all\n\
"), stdout);
      fputs (_("\
      --no-preserve=ATTR_LIST  don't preserve the specified attributes\n\
      --parents                use full source file name under DIRECTORY\n\
"), stdout);
      fputs (_("\
  -R, -r, --recursive          copy directories recursively\n\
      --reflink[=WHEN]         control clone/CoW copies. See below\n\
      --remove-destination     remove each existing destination file before\n\
                                 attempting to open it (contrast with --force)\
\n"), stdout);
      fputs (_("\
      --sparse=WHEN            control creation of sparse files. See below\n\
      --strip-trailing-slashes  remove any trailing slashes from each SOURCE\n\
                                 argument\n\
"), stdout);
      fputs (_("\
  -s, --symbolic-link          make symbolic links instead of copying\n\
  -S, --suffix=SUFFIX          override the usual backup suffix\n\
  -t, --target-directory=DIRECTORY  copy all SOURCE arguments into DIRECTORY\n\
  -T, --no-target-directory    treat DEST as a normal file\n\
"), stdout);
      fputs (_("\
  -u, --update                 copy only when the SOURCE file is newer\n\
                                 than the destination file or when the\n\
                                 destination file is missing\n\
  -v, --verbose                explain what is being done\n\
  -x, --one-file-system        stay on this file system\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
By default, sparse SOURCE files are detected by a crude heuristic and the\n\
corresponding DEST file is made sparse as well.  That is the behavior\n\
selected by --sparse=auto.  Specify --sparse=always to create a sparse DEST\n\
file whenever the SOURCE file contains a long enough sequence of zero bytes.\n\
Use --sparse=never to inhibit creation of sparse files.\n\
\n\
When --reflink[=always] is specified, perform a lightweight copy, where the\n\
data blocks are copied only when modified.  If this is not possible the copy\n\
fails, or if --reflink=auto is specified, fall back to a standard copy.\n\
"), stdout);
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
      fputs (_("\
\n\
As a special case, cp makes a backup of SOURCE when the force and backup\n\
options are given and SOURCE and DEST are the same name for an existing,\n\
regular file.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

