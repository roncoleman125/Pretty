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
"),
              program_name);
      printf (_("\
Print the last %d lines of each FILE to standard output.\n\
With more than one FILE, precede each with a header giving the file name.\n\
With no FILE, or when FILE is -, read standard input.\n\
\n\
"), DEFAULT_N_LINES);
     fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
     fputs (_("\
  -c, --bytes=K            output the last K bytes; alternatively, use -c +K\n\
                           to output bytes starting with the Kth of each file\n\
"), stdout);
     fputs (_("\
  -f, --follow[={name|descriptor}]\n\
                           output appended data as the file grows;\n\
                           -f, --follow, and --follow=descriptor are\n\
                           equivalent\n\
  -F                       same as --follow=name --retry\n\
"), stdout);
     printf (_("\
  -n, --lines=K            output the last K lines, instead of the last %d;\n\
                           or use -n +K to output lines starting with the Kth\n\
      --max-unchanged-stats=N\n\
                           with --follow=name, reopen a FILE which has not\n\
                           changed size after N (default %d) iterations\n\
                           to see if it has been unlinked or renamed\n\
                           (this is the usual case of rotated log files).\n\
                           With inotify, this option is rarely useful.\n\
"),
             DEFAULT_N_LINES,
             DEFAULT_MAX_N_UNCHANGED_STATS_BETWEEN_OPENS
             );
     fputs (_("\
      --pid=PID            with -f, terminate after process ID, PID dies\n\
  -q, --quiet, --silent    never output headers giving file names\n\
      --retry              keep trying to open a file even when it is or\n\
                             becomes inaccessible; useful when following by\n\
                             name, i.e., with --follow=name\n\
"), stdout);
     fputs (_("\
  -s, --sleep-interval=N   with -f, sleep for approximately N seconds\n\
                             (default 1.0) between iterations.\n\
                             With inotify, this option is rarely useful.\n\
  -v, --verbose            always output headers giving file names\n\
"), stdout);
     fputs (HELP_OPTION_DESCRIPTION, stdout);
     fputs (VERSION_OPTION_DESCRIPTION, stdout);
     fputs (_("\
\n\
If the first character of K (the number of bytes or lines) is a `+',\n\
print beginning with the Kth item from the start of each file, otherwise,\n\
print the last K items in the file.  K may have a multiplier suffix:\n\
b 512, kB 1000, K 1024, MB 1000*1000, M 1024*1024,\n\
GB 1000*1000*1000, G 1024*1024*1024, and so on for T, P, E, Z, Y.\n\
\n\
"), stdout);
     fputs (_("\
With --follow (-f), tail defaults to following the file descriptor, which\n\
means that even if a tail'ed file is renamed, tail will continue to track\n\
its end.  This default behavior is not desirable when you really want to\n\
track the actual name of the file, not the file descriptor (e.g., log\n\
rotation).  Use --follow=name in that case.  That causes tail to track the\n\
named file in a way that accommodates renaming, removal and creation.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

