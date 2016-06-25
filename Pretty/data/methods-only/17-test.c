/* This is an implementation of a Posix.2 proposal by David Korn. */
static bool
posixtest (int nargs)
{
  bool value;

  switch (nargs)
    {
      case 1:
        value = one_argument ();
        break;

      case 2:
        value = two_arguments ();
        break;

      case 3:
        value = three_arguments ();
        break;

      case 4:
        if (STREQ (argv[pos], "!"))
          {
            advance (true);
            value = !three_arguments ();
            break;
          }
        if (STREQ (argv[pos], "(") && STREQ (argv[pos + 3], ")"))
          {
            advance (false);
            value = two_arguments ();
            advance (false);
            break;
          }
        /* FALLTHROUGH */
      case 5:
      default:
        if (nargs <= 0)
          abort ();
        value = expr ();
    }

  return (value);
}


void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      fputs (_("\
Usage: test EXPRESSION\n\
  or:  test\n\
  or:  [ EXPRESSION ]\n\
  or:  [ ]\n\
  or:  [ OPTION\n\
"), stdout);
      fputs (_("\
Exit with the status determined by EXPRESSION.\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
An omitted EXPRESSION defaults to false.  Otherwise,\n\
EXPRESSION is true or false and sets exit status.  It is one of:\n\
"), stdout);
      fputs (_("\
\n\
  ( EXPRESSION )               EXPRESSION is true\n\
  ! EXPRESSION                 EXPRESSION is false\n\
  EXPRESSION1 -a EXPRESSION2   both EXPRESSION1 and EXPRESSION2 are true\n\
  EXPRESSION1 -o EXPRESSION2   either EXPRESSION1 or EXPRESSION2 is true\n\
"), stdout);
      fputs (_("\
\n\
  -n STRING            the length of STRING is nonzero\n\
  STRING               equivalent to -n STRING\n\
  -z STRING            the length of STRING is zero\n\
  STRING1 = STRING2    the strings are equal\n\
  STRING1 != STRING2   the strings are not equal\n\
"), stdout);
      fputs (_("\
\n\
  INTEGER1 -eq INTEGER2   INTEGER1 is equal to INTEGER2\n\
  INTEGER1 -ge INTEGER2   INTEGER1 is greater than or equal to INTEGER2\n\
  INTEGER1 -gt INTEGER2   INTEGER1 is greater than INTEGER2\n\
  INTEGER1 -le INTEGER2   INTEGER1 is less than or equal to INTEGER2\n\
  INTEGER1 -lt INTEGER2   INTEGER1 is less than INTEGER2\n\
  INTEGER1 -ne INTEGER2   INTEGER1 is not equal to INTEGER2\n\
"), stdout);
      fputs (_("\
\n\
  FILE1 -ef FILE2   FILE1 and FILE2 have the same device and inode numbers\n\
  FILE1 -nt FILE2   FILE1 is newer (modification date) than FILE2\n\
  FILE1 -ot FILE2   FILE1 is older than FILE2\n\
"), stdout);
      fputs (_("\
\n\
  -b FILE     FILE exists and is block special\n\
  -c FILE     FILE exists and is character special\n\
  -d FILE     FILE exists and is a directory\n\
  -e FILE     FILE exists\n\
"), stdout);
      fputs (_("\
  -f FILE     FILE exists and is a regular file\n\
  -g FILE     FILE exists and is set-group-ID\n\
  -G FILE     FILE exists and is owned by the effective group ID\n\
  -h FILE     FILE exists and is a symbolic link (same as -L)\n\
  -k FILE     FILE exists and has its sticky bit set\n\
"), stdout);
      fputs (_("\
  -L FILE     FILE exists and is a symbolic link (same as -h)\n\
  -O FILE     FILE exists and is owned by the effective user ID\n\
  -p FILE     FILE exists and is a named pipe\n\
  -r FILE     FILE exists and read permission is granted\n\
  -s FILE     FILE exists and has a size greater than zero\n\
"), stdout);
      fputs (_("\
  -S FILE     FILE exists and is a socket\n\
  -t FD       file descriptor FD is opened on a terminal\n\
  -u FILE     FILE exists and its set-user-ID bit is set\n\
  -w FILE     FILE exists and write permission is granted\n\
  -x FILE     FILE exists and execute (or search) permission is granted\n\
"), stdout);
      fputs (_("\
\n\
Except for -h and -L, all FILE-related tests dereference symbolic links.\n\
Beware that parentheses need to be escaped (e.g., by backslashes) for shells.\n\
INTEGER may also be -l STRING, which evaluates to the length of STRING.\n\
"), stdout);
      fputs (_("\
\n\
NOTE: [ honors the --help and --version options, but test does not.\n\
test treats each of those as it treats any other nonempty STRING.\n\
"), stdout);
      printf (USAGE_BUILTIN_WARNING, _("test and/or ["));
      emit_ancillary_info ();
    }
  exit (status);
}


  proper_name ("Kevin Braunsdorf"), \
  proper_name ("Matthew Bradburn")

