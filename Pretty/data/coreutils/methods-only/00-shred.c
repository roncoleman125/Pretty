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
Overwrite the specified FILE(s) repeatedly, in order to make it harder\n\
for even very expensive hardware probing to recover the data.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      printf (_("\
  -f, --force    change permissions to allow writing if necessary\n\
  -n, --iterations=N  overwrite N times instead of the default (%d)\n\
      --random-source=FILE  get random bytes from FILE\n\
  -s, --size=N   shred this many bytes (suffixes like K, M, G accepted)\n\
"), DEFAULT_PASSES);
      fputs (_("\
  -u, --remove   truncate and remove file after overwriting\n\
  -v, --verbose  show progress\n\
  -x, --exact    do not round file sizes up to the next full block;\n\
                   this is the default for non-regular files\n\
  -z, --zero     add a final overwrite with zeros to hide shredding\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
If FILE is -, shred standard output.\n\
\n\
Delete FILE(s) if --remove (-u) is specified.  The default is not to remove\n\
the files because it is common to operate on device files like /dev/hda,\n\
and those files usually should not be removed.  When operating on regular\n\
files, most people use the --remove option.\n\
\n\
"), stdout);
      fputs (_("\
CAUTION: Note that shred relies on a very important assumption:\n\
that the file system overwrites data in place.  This is the traditional\n\
way to do things, but many modern file system designs do not satisfy this\n\
assumption.  The following are examples of file systems on which shred is\n\
not effective, or is not guaranteed to be effective in all file system modes:\n\
\n\
"), stdout);
      fputs (_("\
* log-structured or journaled file systems, such as those supplied with\n\
AIX and Solaris (and JFS, ReiserFS, XFS, Ext3, etc.)\n\
\n\
* file systems that write redundant data and carry on even if some writes\n\
fail, such as RAID-based file systems\n\
\n\
* file systems that make snapshots, such as Network Appliance's NFS server\n\
\n\
"), stdout);
      fputs (_("\
* file systems that cache in temporary locations, such as NFS\n\
version 3 clients\n\
\n\
* compressed file systems\n\
\n\
"), stdout);
      fputs (_("\
In the case of ext3 file systems, the above disclaimer applies\n\
(and shred is thus of limited effectiveness) only in data=journal mode,\n\
which journals file data in addition to just metadata.  In both the\n\
data=ordered (default) and data=writeback modes, shred works as usual.\n\
Ext3 journaling modes can be changed by adding the data=something option\n\
to the mount options for a particular file system in the /etc/fstab file,\n\
as documented in the mount man page (man mount).\n\
\n\
"), stdout);
      fputs (_("\
In addition, file system backups and remote mirrors may contain copies\n\
of the file that cannot be removed, and that will allow a shredded file\n\
to be recovered later.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}


