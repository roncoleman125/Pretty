void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... CONTEXT FILE...\n\
  or:  %s [OPTION]... [-u USER] [-r ROLE] [-l RANGE] [-t TYPE] FILE...\n\
  or:  %s [OPTION]... --reference=RFILE FILE...\n\
"),
        program_name, program_name, program_name);
      fputs (_("\
Change the security context of each FILE to CONTEXT.\n\
With --reference, change the security context of each FILE to that of RFILE.\n\
\n\
  -h, --no-dereference   affect symbolic links instead of any referenced file\n\
"), stdout);
      fputs (_("\
      --reference=RFILE  use RFILE's security context rather than specifying\n\
                         a CONTEXT value\n\
  -R, --recursive        operate on files and directories recursively\n\
  -v, --verbose          output a diagnostic for every file processed\n\
"), stdout);
      fputs (_("\
  -u, --user=USER        set user USER in the target security context\n\
  -r, --role=ROLE        set role ROLE in the target security context\n\
  -t, --type=TYPE        set type TYPE in the target security context\n\
  -l, --range=RANGE      set range RANGE in the target security context\n\
\n\
"), stdout);
      fputs (_("\
The following options modify how a hierarchy is traversed when the -R\n\
option is also specified.  If more than one is specified, only the final\n\
one takes effect.\n\
\n\
  -H                     if a command line argument is a symbolic link\n\
                         to a directory, traverse it\n\
  -L                     traverse every symbolic link to a directory\n\
                         encountered\n\
  -P                     do not traverse any symbolic links (default)\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

