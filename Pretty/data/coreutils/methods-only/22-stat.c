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
Display file or file system status.\n\
\n\
  -L, --dereference     follow links\n\
  -f, --file-system     display file system status instead of file status\n\
"), stdout);
      fputs (_("\
  -c  --format=FORMAT   use the specified FORMAT instead of the default;\n\
                          output a newline after each use of FORMAT\n\
      --printf=FORMAT   like --format, but interpret backslash escapes,\n\
                          and do not output a mandatory trailing newline.\n\
                          If you want a newline, include \\n in FORMAT\n\
  -t, --terse           print the information in terse form\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);

      fputs (_("\n\
The valid format sequences for files (without --file-system):\n\
\n\
  %a   Access rights in octal\n\
  %A   Access rights in human readable form\n\
  %b   Number of blocks allocated (see %B)\n\
  %B   The size in bytes of each block reported by %b\n\
  %C   SELinux security context string\n\
"), stdout);
      fputs (_("\
  %d   Device number in decimal\n\
  %D   Device number in hex\n\
  %f   Raw mode in hex\n\
  %F   File type\n\
  %g   Group ID of owner\n\
  %G   Group name of owner\n\
"), stdout);
      fputs (_("\
  %h   Number of hard links\n\
  %i   Inode number\n\
  %m   Mount point\n\
  %n   File name\n\
  %N   Quoted file name with dereference if symbolic link\n\
  %o   I/O block size\n\
  %s   Total size, in bytes\n\
  %t   Major device type in hex\n\
  %T   Minor device type in hex\n\
"), stdout);
      fputs (_("\
  %u   User ID of owner\n\
  %U   User name of owner\n\
  %w   Time of file birth, human-readable; - if unknown\n\
  %W   Time of file birth, seconds since Epoch; 0 if unknown\n\
  %x   Time of last access, human-readable\n\
  %X   Time of last access, seconds since Epoch\n\
  %y   Time of last modification, human-readable\n\
  %Y   Time of last modification, seconds since Epoch\n\
  %z   Time of last change, human-readable\n\
  %Z   Time of last change, seconds since Epoch\n\
\n\
"), stdout);

      fputs (_("\
Valid format sequences for file systems:\n\
\n\
  %a   Free blocks available to non-superuser\n\
  %b   Total data blocks in file system\n\
  %c   Total file nodes in file system\n\
  %d   Free file nodes in file system\n\
  %f   Free blocks in file system\n\
"), stdout);
      fputs (_("\
  %i   File System ID in hex\n\
  %l   Maximum length of filenames\n\
  %n   File name\n\
  %s   Block size (for faster transfers)\n\
  %S   Fundamental block size (for block counts)\n\
  %t   Type in hex\n\
  %T   Type in human readable form\n\
"), stdout);
      printf (USAGE_BUILTIN_WARNING, PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

