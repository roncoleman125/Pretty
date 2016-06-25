void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s FILE1 FILE2\n\
  or:  %s OPTION\n"), program_name, program_name);
      fputs (_("Call the link function to create a link named FILE2\
 to an existing FILE1.\n\n"),
             stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      emit_ancillary_info ();
    }
  exit (status);
}

