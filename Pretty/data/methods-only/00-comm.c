void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... FILE1 FILE2\n\
"),
              program_name);
      fputs (_("\
Compare sorted files FILE1 and FILE2 line by line.\n\
"), stdout);
      fputs (_("\
\n\
With no options, produce three-column output.  Column one contains\n\
lines unique to FILE1, column two contains lines unique to FILE2,\n\
and column three contains lines common to both files.\n\
"), stdout);
      fputs (_("\
\n\
  -1              suppress column 1 (lines unique to FILE1)\n\
  -2              suppress column 2 (lines unique to FILE2)\n\
  -3              suppress column 3 (lines that appear in both files)\n\
"), stdout);
      fputs (_("\
\n\
  --check-order     check that the input is correctly sorted, even\n\
                      if all input lines are pairable\n\
  --nocheck-order   do not check that the input is correctly sorted\n\
"), stdout);
      fputs (_("\
  --output-delimiter=STR  separate columns with STR\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Note, comparisons honor the rules specified by `LC_COLLATE'.\n\
"), stdout);
      printf (_("\
\n\
Examples:\n\
  %s -12 file1 file2  Print only lines present in both file1 and file2.\n\
  %s -3 file1 file2  Print lines in file1 not in file2, and vice versa.\n\
"),
              program_name, program_name);
      emit_ancillary_info ();
    }
  exit (status);
}

