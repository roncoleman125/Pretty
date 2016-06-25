static void
write_error (void)
{
  error (EXIT_FAILURE, errno, _("write error"));
  abort ();
}

