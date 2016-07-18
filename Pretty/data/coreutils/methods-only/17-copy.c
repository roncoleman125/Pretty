/* A wrapper around "setfscreatecon (NULL)" that exits upon failure.  */
static void
restore_default_fscreatecon_or_die (void)
{
  if (setfscreatecon (NULL) != 0)
    error (EXIT_FAILURE, errno,
           _("failed to restore the default file creation context"));
}

