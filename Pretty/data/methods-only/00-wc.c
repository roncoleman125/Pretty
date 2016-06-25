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
"),
              program_name, program_name);
      fputs (_("\
Print newline, word, and byte counts for each FILE, and a total line if\n\
more than one FILE is specified.  With no FILE, or when FILE is -,\n\
read standard input.  A word is a non-zero-length sequence of characters\n\
delimited by white space.\n\
  -c, --bytes            print the byte counts\n\
  -m, --chars            print the character counts\n\
  -l, --lines            print the newline counts\n\
"), stdout);
      fputs (_("\
      --files0-from=F    read input from the files specified by\n\
                           NUL-terminated names in file F;\n\
                           If F is - then read names from standard input\n\
  -L, --max-line-length  print the length of the longest line\n\
  -w, --words            print the word counts\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

