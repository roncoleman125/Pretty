static bool
unary_operator (void)
{
  struct stat stat_buf;

  switch (argv[pos][1])
    {
    default:
      return false;

      /* All of the following unary operators use unary_advance (), which
         checks to make sure that there is an argument, and then advances
         pos right past it.  This means that pos - 1 is the location of the
         argument. */

    case 'a':			/* file exists in the file system? */
    case 'e':
      unary_advance ();
      return stat (argv[pos - 1], &stat_buf) == 0;

    case 'r':			/* file is readable? */
      unary_advance ();
      return euidaccess (argv[pos - 1], R_OK) == 0;

    case 'w':			/* File is writable? */
      unary_advance ();
      return euidaccess (argv[pos - 1], W_OK) == 0;

    case 'x':			/* File is executable? */
      unary_advance ();
      return euidaccess (argv[pos - 1], X_OK) == 0;

    case 'O':			/* File is owned by you? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && (geteuid () == stat_buf.st_uid));

    case 'G':			/* File is owned by your group? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && (getegid () == stat_buf.st_gid));

    case 'f':			/* File is a file? */
      unary_advance ();
      /* Under POSIX, -f is true if the given file exists
         and is a regular file. */
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISREG (stat_buf.st_mode));

    case 'd':			/* File is a directory? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISDIR (stat_buf.st_mode));

    case 's':			/* File has something in it? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && 0 < stat_buf.st_size);

    case 'S':			/* File is a socket? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISSOCK (stat_buf.st_mode));

    case 'c':			/* File is character special? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISCHR (stat_buf.st_mode));

    case 'b':			/* File is block special? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISBLK (stat_buf.st_mode));

    case 'p':			/* File is a named pipe? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && S_ISFIFO (stat_buf.st_mode));

    case 'L':			/* Same as -h  */
      /*FALLTHROUGH*/

    case 'h':			/* File is a symbolic link? */
      unary_advance ();
      return (lstat (argv[pos - 1], &stat_buf) == 0
              && S_ISLNK (stat_buf.st_mode));

    case 'u':			/* File is setuid? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && (stat_buf.st_mode & S_ISUID));

    case 'g':			/* File is setgid? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && (stat_buf.st_mode & S_ISGID));

    case 'k':			/* File has sticky bit set? */
      unary_advance ();
      return (stat (argv[pos - 1], &stat_buf) == 0
              && (stat_buf.st_mode & S_ISVTX));

    case 't':			/* File (fd) is a terminal? */
      {
        long int fd;
        char const *arg;
        unary_advance ();
        arg = find_int (argv[pos - 1]);
        errno = 0;
        fd = strtol (arg, NULL, 10);
        return (errno != ERANGE && 0 <= fd && fd <= INT_MAX && isatty (fd));
      }

    case 'n':			/* True if arg has some length. */
      unary_advance ();
      return argv[pos - 1][0] != 0;

    case 'z':			/* True if arg has no length. */
      unary_advance ();
      return argv[pos - 1][0] == '\0';
    }
}

