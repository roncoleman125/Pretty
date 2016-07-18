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
List information about the FILEs (the current directory by default).\n\
Sort entries alphabetically if none of -cftuvSUX nor --sort.\n\
\n\
"), stdout);
      fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
      fputs (_("\
  -a, --all                  do not ignore entries starting with .\n\
  -A, --almost-all           do not list implied . and ..\n\
      --author               with -l, print the author of each file\n\
  -b, --escape               print C-style escapes for nongraphic characters\n\
"), stdout);
      fputs (_("\
      --block-size=SIZE      scale sizes by SIZE before printing them.  E.g.,\n\
                               `--block-size=M' prints sizes in units of\n\
                               1,048,576 bytes.  See SIZE format below.\n\
  -B, --ignore-backups       do not list implied entries ending with ~\n\
  -c                         with -lt: sort by, and show, ctime (time of last\n\
                               modification of file status information)\n\
                               with -l: show ctime and sort by name\n\
                               otherwise: sort by ctime\n\
"), stdout);
      fputs (_("\
  -C                         list entries by columns\n\
      --color[=WHEN]         colorize the output.  WHEN defaults to `always'\n\
                               or can be `never' or `auto'.  More info below\n\
  -d, --directory            list directory entries instead of contents,\n\
                               and do not dereference symbolic links\n\
  -D, --dired                generate output designed for Emacs' dired mode\n\
"), stdout);
      fputs (_("\
  -f                         do not sort, enable -aU, disable -ls --color\n\
  -F, --classify             append indicator (one of */=>@|) to entries\n\
      --file-type            likewise, except do not append `*'\n\
      --format=WORD          across -x, commas -m, horizontal -x, long -l,\n\
                               single-column -1, verbose -l, vertical -C\n\
      --full-time            like -l --time-style=full-iso\n\
"), stdout);
      fputs (_("\
  -g                         like -l, but do not list owner\n\
"), stdout);
      fputs (_("\
      --group-directories-first\n\
                             group directories before files.\n\
                               augment with a --sort option, but any\n\
                               use of --sort=none (-U) disables grouping\n\
"), stdout);
      fputs (_("\
  -G, --no-group             in a long listing, don't print group names\n\
  -h, --human-readable       with -l, print sizes in human readable format\n\
                               (e.g., 1K 234M 2G)\n\
      --si                   likewise, but use powers of 1000 not 1024\n\
"), stdout);
      fputs (_("\
  -H, --dereference-command-line\n\
                             follow symbolic links listed on the command line\n\
      --dereference-command-line-symlink-to-dir\n\
                             follow each command line symbolic link\n\
                             that points to a directory\n\
      --hide=PATTERN         do not list implied entries matching shell PATTERN\
\n\
                               (overridden by -a or -A)\n\
"), stdout);
      fputs (_("\
      --indicator-style=WORD  append indicator with style WORD to entry names:\
\n\
                               none (default), slash (-p),\n\
                               file-type (--file-type), classify (-F)\n\
  -i, --inode                print the index number of each file\n\
  -I, --ignore=PATTERN       do not list implied entries matching shell PATTERN\
\n\
  -k                         like --block-size=1K\n\
"), stdout);
      fputs (_("\
  -l                         use a long listing format\n\
  -L, --dereference          when showing file information for a symbolic\n\
                               link, show information for the file the link\n\
                               references rather than for the link itself\n\
  -m                         fill width with a comma separated list of entries\
\n\
"), stdout);
      fputs (_("\
  -n, --numeric-uid-gid      like -l, but list numeric user and group IDs\n\
  -N, --literal              print raw entry names (don't treat e.g. control\n\
                               characters specially)\n\
  -o                         like -l, but do not list group information\n\
  -p, --indicator-style=slash\n\
                             append / indicator to directories\n\
"), stdout);
      fputs (_("\
  -q, --hide-control-chars   print ? instead of non graphic characters\n\
      --show-control-chars   show non graphic characters as-is (default\n\
                             unless program is `ls' and output is a terminal)\n\
  -Q, --quote-name           enclose entry names in double quotes\n\
      --quoting-style=WORD   use quoting style WORD for entry names:\n\
                               literal, locale, shell, shell-always, c, escape\
\n\
"), stdout);
      fputs (_("\
  -r, --reverse              reverse order while sorting\n\
  -R, --recursive            list subdirectories recursively\n\
  -s, --size                 print the allocated size of each file, in blocks\n\
"), stdout);
      fputs (_("\
  -S                         sort by file size\n\
      --sort=WORD            sort by WORD instead of name: none -U,\n\
                             extension -X, size -S, time -t, version -v\n\
      --time=WORD            with -l, show time as WORD instead of modification\
\n\
                             time: atime -u, access -u, use -u, ctime -c,\n\
                             or status -c; use specified time as sort key\n\
                             if --sort=time\n\
"), stdout);
      fputs (_("\
      --time-style=STYLE     with -l, show times using style STYLE:\n\
                             full-iso, long-iso, iso, locale, +FORMAT.\n\
                             FORMAT is interpreted like `date'; if FORMAT is\n\
                             FORMAT1<newline>FORMAT2, FORMAT1 applies to\n\
                             non-recent files and FORMAT2 to recent files;\n\
                             if STYLE is prefixed with `posix-', STYLE\n\
                             takes effect only outside the POSIX locale\n\
"), stdout);
      fputs (_("\
  -t                         sort by modification time\n\
  -T, --tabsize=COLS         assume tab stops at each COLS instead of 8\n\
"), stdout);
      fputs (_("\
  -u                         with -lt: sort by, and show, access time\n\
                               with -l: show access time and sort by name\n\
                               otherwise: sort by access time\n\
  -U                         do not sort; list entries in directory order\n\
  -v                         natural sort of (version) numbers within text\n\
"), stdout);
      fputs (_("\
  -w, --width=COLS           assume screen width instead of current value\n\
  -x                         list entries by lines instead of by columns\n\
  -X                         sort alphabetically by entry extension\n\
  -Z, --context              print any SELinux security context of each file\n\
  -1                         list one file per line\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_size_note ();
      fputs (_("\
\n\
Using color to distinguish file types is disabled both by default and\n\
with --color=never.  With --color=auto, ls emits color codes only when\n\
standard output is connected to a terminal.  The LS_COLORS environment\n\
variable can change the settings.  Use the dircolors command to set it.\n\
"), stdout);
      fputs (_("\
\n\
Exit status:\n\
 0  if OK,\n\
 1  if minor problems (e.g., cannot access subdirectory),\n\
 2  if serious trouble (e.g., cannot access command-line argument).\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}
