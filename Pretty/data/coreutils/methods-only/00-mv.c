static void
rm_option_init (struct rm_options *x)
{
  x->ignore_missing_files = false;
  x->recursive = true;
  x->one_file_system = false;

  /* Should we prompt for removal, too?  No.  Prompting for the `move'
     part is enough.  It implies removal.  */
  x->interactive = RMI_NEVER;
  x->stdin_tty = false;

  x->verbose = false;

  /* Since this program may well have to process additional command
     line arguments after any call to `rm', that function must preserve
     the initial working directory, in case one of those is a
     `.'-relative name.  */
  x->require_restore_cwd = true;

  {
    static struct dev_ino dev_ino_buf;
    x->root_dev_ino = get_root_dev_ino (&dev_ino_buf);
    if (x->root_dev_ino == NULL)
      error (EXIT_FAILURE, errno, _("failed to get attributes of %s"),
             quote ("/"));
  }
}

