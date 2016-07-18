void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPERAND]...\n\
  or:  %s OPTION\n\
"),
              program_name, program_name);
      fputs (_("\
Copy a file, converting and formatting according to the operands.\n\
\n\
  bs=BYTES        read and write BYTES bytes at a time (also see ibs=,obs=)\n\
  cbs=BYTES       convert BYTES bytes at a time\n\
  conv=CONVS      convert the file as per the comma separated symbol list\n\
  count=BLOCKS    copy only BLOCKS input blocks\n\
  ibs=BYTES       read BYTES bytes at a time (default: 512)\n\
"), stdout);
      fputs (_("\
  if=FILE         read from FILE instead of stdin\n\
  iflag=FLAGS     read as per the comma separated symbol list\n\
  obs=BYTES       write BYTES bytes at a time (default: 512)\n\
  of=FILE         write to FILE instead of stdout\n\
  oflag=FLAGS     write as per the comma separated symbol list\n\
  seek=BLOCKS     skip BLOCKS obs-sized blocks at start of output\n\
  skip=BLOCKS     skip BLOCKS ibs-sized blocks at start of input\n\
  status=noxfer   suppress transfer statistics\n\
"), stdout);
      fputs (_("\
\n\
BLOCKS and BYTES may be followed by the following multiplicative suffixes:\n\
c =1, w =2, b =512, kB =1000, K =1024, MB =1000*1000, M =1024*1024, xM =M\n\
GB =1000*1000*1000, G =1024*1024*1024, and so on for T, P, E, Z, Y.\n\
\n\
Each CONV symbol may be:\n\
\n\
"), stdout);
      fputs (_("\
  ascii     from EBCDIC to ASCII\n\
  ebcdic    from ASCII to EBCDIC\n\
  ibm       from ASCII to alternate EBCDIC\n\
  block     pad newline-terminated records with spaces to cbs-size\n\
  unblock   replace trailing spaces in cbs-size records with newline\n\
  lcase     change upper case to lower case\n\
"), stdout);
      fputs (_("\
  nocreat   do not create the output file\n\
  excl      fail if the output file already exists\n\
  notrunc   do not truncate the output file\n\
  ucase     change lower case to upper case\n\
  swab      swap every pair of input bytes\n\
"), stdout);
      fputs (_("\
  noerror   continue after read errors\n\
  sync      pad every input block with NULs to ibs-size; when used\n\
            with block or unblock, pad with spaces rather than NULs\n\
  fdatasync  physically write output file data before finishing\n\
  fsync     likewise, but also write metadata\n\
"), stdout);
      fputs (_("\
\n\
Each FLAG symbol may be:\n\
\n\
  append    append mode (makes sense only for output; conv=notrunc suggested)\n\
"), stdout);
      if (O_CIO)
        fputs (_("  cio       use concurrent I/O for data\n"), stdout);
      if (O_DIRECT)
        fputs (_("  direct    use direct I/O for data\n"), stdout);
      if (O_DIRECTORY)
        fputs (_("  directory  fail unless a directory\n"), stdout);
      if (O_DSYNC)
        fputs (_("  dsync     use synchronized I/O for data\n"), stdout);
      if (O_SYNC)
        fputs (_("  sync      likewise, but also for metadata\n"), stdout);
      fputs (_("  fullblock  accumulate full blocks of input (iflag only)\n"),
             stdout);
      if (O_NONBLOCK)
        fputs (_("  nonblock  use non-blocking I/O\n"), stdout);
      if (O_NOATIME)
        fputs (_("  noatime   do not update access time\n"), stdout);
      if (O_NOCTTY)
        fputs (_("  noctty    do not assign controlling terminal from file\n"),
               stdout);
      if (HAVE_WORKING_O_NOFOLLOW)
        fputs (_("  nofollow  do not follow symlinks\n"), stdout);
      if (O_NOLINKS)
        fputs (_("  nolinks   fail if multiply-linked\n"), stdout);
      if (O_BINARY)
        fputs (_("  binary    use binary I/O for data\n"), stdout);
      if (O_TEXT)
        fputs (_("  text      use text I/O for data\n"), stdout);

      {
        char const *siginfo_name = (SIGINFO == SIGUSR1 ? "USR1" : "INFO");
        printf (_("\
\n\
Sending a %s signal to a running `dd' process makes it\n\
print I/O statistics to standard error and then resume copying.\n\
\n\
  $ dd if=/dev/zero of=/dev/null& pid=$!\n\
  $ kill -%s $pid; sleep 1; kill $pid\n\
  18335302+0 records in\n\
  18335302+0 records out\n\
  9387674624 bytes (9.4 GB) copied, 34.6279 seconds, 271 MB/s\n\
\n\
Options are:\n\
\n\
"),
                siginfo_name, siginfo_name);
      }

      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

