void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("Usage: %s [OPTION]... [FILE]...\n"), program_name);
      fputs (_("\
Show information about the file system on which each FILE resides,\n\
or all file systems by default.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -a, --all             include dummy file systems\n\
  -B, --block-size=SIZE  scale sizes by SIZE before printing them.  E.g.,\n\
                           `-BM' prints sizes in units of 1,048,576 bytes.\n\
                           See SIZE format below.\n\
      --total           produce a grand total\n\
  -h, --human-readable  print sizes in human readable format (e.g., 1K 234M 2G)\
\n\
  -H, --si              likewise, but use powers of 1000 not 1024\n\
"), stdout);
      fputs (_("\
  -i, --inodes          list inode information instead of block usage\n\
  -k                    like --block-size=1K\n\
  -l, --local           limit listing to local file systems\n\
      --no-sync         do not invoke sync before getting usage info (default)\
\n\
"), stdout);
      fputs (_("\
  -P, --portability     use the POSIX output format\n\
      --sync            invoke sync before getting usage info\n\
  -t, --type=TYPE       limit listing to file systems of type TYPE\n\
  -T, --print-type      print file system type\n\
  -x, --exclude-type=TYPE   limit listing to file systems not of type TYPE\n\
  -v                    (ignored)\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_blocksize_note ("DF");
      emit_size_note ();
      emit_ancillary_info ();
    }
  exit (status);
}

