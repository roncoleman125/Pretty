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
  or:  %s [OPTION]... -d DIRECTORY...\n\
"),
              program_name, program_name, program_name, program_name);
      fputs (_("\
\n\
This install program copies files (often just compiled) into destination\n\
locations you choose.  If you want to download and install a ready-to-use\n\
package on a GNU/Linux system, you should instead be using a package manager\n\
like yum(1) or apt-get(1).\n\
\n\
In the first three forms, copy SOURCE to DEST or multiple SOURCE(s) to\n\
the existing DIRECTORY, while setting permission modes and owner/group.\n\
In the 4th form, create all components of the given DIRECTORY(ies).\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
      --backup[=CONTROL]  make a backup of each existing destination file\n\
  -b                  like --backup but does not accept an argument\n\
  -c                  (ignored)\n\
  -C, --compare       compare each pair of source and destination files, and\n\
                        in some cases, do not modify the destination at all\n\
  -d, --directory     treat all arguments as directory names; create all\n\
                        components of the specified directories\n\
"), stdout);
      fputs (_("\
  -D                  create all leading components of DEST except the last,\n\
                        then copy SOURCE to DEST\n\
  -g, --group=GROUP   set group ownership, instead of process' current group\n\
  -m, --mode=MODE     set permission mode (as in chmod), instead of rwxr-xr-x\n\
  -o, --owner=OWNER   set ownership (super-user only)\n\
"), stdout);
      fputs (_("\
  -p, --preserve-timestamps   apply access/modification times of SOURCE files\n\
                        to corresponding destination files\n\
  -s, --strip         strip symbol tables\n\
      --strip-program=PROGRAM  program used to strip binaries\n\
  -S, --suffix=SUFFIX  override the usual backup suffix\n\
  -t, --target-directory=DIRECTORY  copy all SOURCE arguments into DIRECTORY\n\
  -T, --no-target-directory  treat DEST as a normal file\n\
  -v, --verbose       print the name of each directory as it is created\n\
"), stdout);
      fputs (_("\
      --preserve-context  preserve SELinux security context\n\
  -Z, --context=CONTEXT  set SELinux security context of files and directories\
\n\
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
