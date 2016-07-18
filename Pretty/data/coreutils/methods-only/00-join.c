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
For each pair of input lines with identical join fields, write a line to\n\
standard output.  The default join field is the first, delimited\n\
by whitespace.  When FILE1 or FILE2 (not both) is -, read standard input.\n\
\n\
  -a FILENUM        print unpairable lines coming from file FILENUM, where\n\
                      FILENUM is 1 or 2, corresponding to FILE1 or FILE2\n\
  -e EMPTY          replace missing input fields with EMPTY\n\
"), stdout);
      fputs (_("\
  -i, --ignore-case  ignore differences in case when comparing fields\n\
  -j FIELD          equivalent to `-1 FIELD -2 FIELD'\n\
  -o FORMAT         obey FORMAT while constructing output line\n\
  -t CHAR           use CHAR as input and output field separator\n\
"), stdout);
      fputs (_("\
  -v FILENUM        like -a FILENUM, but suppress joined output lines\n\
  -1 FIELD          join on this FIELD of file 1\n\
  -2 FIELD          join on this FIELD of file 2\n\
  --check-order     check that the input is correctly sorted, even\n\
                      if all input lines are pairable\n\
  --nocheck-order   do not check that the input is correctly sorted\n\
  --header          treat the first line in each file as field headers,\n\
                      print them without trying to pair them\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Unless -t CHAR is given, leading blanks separate fields and are ignored,\n\
else fields are separated by CHAR.  Any FIELD is a field number counted\n\
from 1.  FORMAT is one or more comma or blank separated specifications,\n\
each being `FILENUM.FIELD' or `0'.  Default FORMAT outputs the join field,\n\
the remaining fields from FILE1, the remaining fields from FILE2, all\n\
separated by CHAR.  If FORMAT is the keyword 'auto', then the first\n\
line of each file determines the number of fields output for each line.\n\
\n\
Important: FILE1 and FILE2 must be sorted on the join fields.\n\
E.g., use ` sort -k 1b,1 ' if `join' has no options,\n\
or use ` join -t '' ' if `sort' has no options.\n\
Note, comparisons honor the rules specified by `LC_COLLATE'.\n\
If the input is not sorted and some lines cannot be joined, a\n\
warning message will be given.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

