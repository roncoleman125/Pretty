/* Open NAME as standard input.  */

static void
set_input_file (const char *name)
{
  if (! STREQ (name, "-") && fd_reopen (STDIN_FILENO, name, O_RDONLY, 0) < 0)
    error (EXIT_FAILURE, errno, _("cannot open %s for reading"), quote (name));
}

