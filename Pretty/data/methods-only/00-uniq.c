void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [INPUT [OUTPUT]]\n\
"),
              program_name);
      fputs (_("\
Filter adjacent matching lines from INPUT (or standard input),\n\
writing to OUTPUT (or standard output).\n\
\n\
With no options, matching lines are merged to the first occurrence.\n\
\n\
"), stdout);
     fputs (_("\
Mandatory arguments to long options are mandatory for short options too.\n\
"), stdout);
     fputs (_("\
  -c, --count           prefix lines by the number of occurrences\n\
  -d, --repeated        only print duplicate lines\n\
"), stdout);
     fputs (_("\
  -D, --all-repeated[=delimit-method]  print all duplicate lines\n\
                        delimit-method={none(default),prepend,separate}\n\
                        Delimiting is done with blank lines\n\
  -f, --skip-fields=N   avoid comparing the first N fields\n\
  -i, --ignore-case     ignore differences in case when comparing\n\
  -s, --skip-chars=N    avoid comparing the first N characters\n\
  -u, --unique          only print unique lines\n\
  -z, --zero-terminated  end lines with 0 byte, not newline\n\
"), stdout);
     fputs (_("\
  -w, --check-chars=N   compare no more than N characters in lines\n\
"), stdout);
     fputs (HELP_OPTION_DESCRIPTION, stdout);
     fputs (VERSION_OPTION_DESCRIPTION, stdout);
     fputs (_("\
\n\
A field is a run of blanks (usually spaces and/or TABs), then non-blank\n\
characters.  Fields are skipped before chars.\n\
"), stdout);
     fputs (_("\
\n\
Note: 'uniq' does not detect repeated lines unless they are adjacent.\n\
You may want to sort the input first, or use `sort -u' without `uniq'.\n\
Also, comparisons honor the rules specified by `LC_COLLATE'.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

