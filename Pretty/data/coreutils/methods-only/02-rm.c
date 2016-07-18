static void
rm_option_init (struct rm_options *x)
{
  x->ignore_missing_files = false;
  x->interactive = RMI_SOMETIMES;
  x->one_file_system = false;
  x->recursive = false;
  x->root_dev_ino = NULL;
  x->stdin_tty = isatty (STDIN_FILENO);
  x->verbose = false;

  /* Since this program exits immediately after calling `rm', rm need not
     expend unnecessary effort to preserve the initial working directory.  */
  x->require_restore_cwd = false;
}

