extern void
xalloc_die (void)
{
  error (0, 0, "%s", _("memory exhausted"));
  cleanup_fatal ();
}

