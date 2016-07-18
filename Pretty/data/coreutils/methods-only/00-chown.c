void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [OPTION]... [OWNER][:[GROUP]] FILE...\n\
  or:  %s [OPTION]... --reference=RFILE FILE...\n\
"),
              program_name, program_name);
      fputs (_("\
Change the owner and/or group of each FILE to OWNER and/or GROUP.\n\
With --reference, change the owner and group of each FILE to those of RFILE.\n\
\n\
  -c, --changes          like verbose but report only when a change is made\n\
      --dereference      affect the referent of each symbolic link (this is\n\
                         the default), rather than the symbolic link itself\n\
"), stdout);
      fputs (_("\
  -h, --no-dereference   affect each symbolic link instead of any referenced\n\
                         file (useful only on systems that can change the\n\
                         ownership of a symlink)\n\
"), stdout);
      fputs (_("\
      --from=CURRENT_OWNER:CURRENT_GROUP\n\
                         change the owner and/or group of each file only if\n\
                         its current owner and/or group match those specified\n\
                         here.  Either may be omitted, in which case a match\n\
                         is not required for the omitted attribute\n\
"), stdout);
      fputs (_("\
      --no-preserve-root  do not treat `/' specially (the default)\n\
      --preserve-root    fail to operate recursively on `/'\n\
"), stdout);
      fputs (_("\
  -f, --silent, --quiet  suppress most error messages\n\
      --reference=RFILE  use RFILE's owner and group rather than\n\
                         specifying OWNER:GROUP values\n\
  -R, --recursive        operate on files and directories recursively\n\
  -v, --verbose          output a diagnostic for every file processed\n\
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
      fputs (_("\
\n\
Owner is unchanged if missing.  Group is unchanged if missing, but changed\n\
to login group if implied by a `:' following a symbolic OWNER.\n\
OWNER and GROUP may be numeric as well as symbolic.\n\
"), stdout);
      printf (_("\
\n\
Examples:\n\
  %s root /u        Change the owner of /u to \"root\".\n\
  %s root:staff /u  Likewise, but also change its group to \"staff\".\n\
  %s -hR root /u    Change the owner of /u and subfiles to \"root\".\n\
"),
              program_name, program_name, program_name);
      emit_ancillary_info ();
    }
  exit (status);
}

