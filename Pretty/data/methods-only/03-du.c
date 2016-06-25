void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [FILE]...\n\
  or:  %s [OPTION]... --files0-from=F\n\
"), program_name, program_name);
      fputs (_("\
Summarize disk usage of each FILE, recursively for directories.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -a, --all             write counts for all files, not just directories\n\
      --apparent-size   print apparent sizes, rather than disk usage; although\
\n\
                          the apparent size is usually smaller, it may be\n\
                          larger due to holes in (`sparse') files, internal\n\
                          fragmentation, indirect blocks, and the like\n\
"), stdout);
      fputs (_("\
  -B, --block-size=SIZE  scale sizes by SIZE before printing them.  E.g.,\n\
                           `-BM' prints sizes in units of 1,048,576 bytes.\n\
                           See SIZE format below.\n\
  -b, --bytes           equivalent to `--apparent-size --block-size=1'\n\
  -c, --total           produce a grand total\n\
  -D, --dereference-args  dereference only symlinks that are listed on the\n\
                          command line\n\
"), stdout);
      fputs (_("\
      --files0-from=F   summarize disk usage of the NUL-terminated file\n\
                          names specified in file F;\n\
                          If F is - then read names from standard input\n\
  -H                    equivalent to --dereference-args (-D)\n\
  -h, --human-readable  print sizes in human readable format (e.g., 1K 234M 2G)\
\n\
      --si              like -h, but use powers of 1000 not 1024\n\
"), stdout);
      fputs (_("\
  -k                    like --block-size=1K\n\
  -l, --count-links     count sizes many times if hard linked\n\
  -m                    like --block-size=1M\n\
"), stdout);
      fputs (_("\
  -L, --dereference     dereference all symbolic links\n\
  -P, --no-dereference  don't follow any symbolic links (this is the default)\n\
  -0, --null            end each output line with 0 byte rather than newline\n\
  -S, --separate-dirs   do not include size of subdirectories\n\
  -s, --summarize       display only a total for each argument\n\
"), stdout);
      fputs (_("\
  -x, --one-file-system    skip directories on different file systems\n\
  -X, --exclude-from=FILE  exclude files that match any pattern in FILE\n\
      --exclude=PATTERN    exclude files that match PATTERN\n\
  -d, --max-depth=N     print the total for a directory (or file, with --all)\n\
                          only if it is N or fewer levels below the command\n\
                          line argument;  --max-depth=0 is the same as\n\
                          --summarize\n\
"), stdout);
      fputs (_("\
      --time            show time of the last modification of any file in the\n\
                          directory, or any of its subdirectories\n\
      --time=WORD       show time as WORD instead of modification time:\n\
                          atime, access, use, ctime or status\n\
      --time-style=STYLE  show times using style STYLE:\n\
                          full-iso, long-iso, iso, +FORMAT\n\
                          FORMAT is interpreted like `date'\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_blocksize_note ("DU");
      emit_size_note ();
      emit_ancillary_info ();
    }
  exit (status);
}

