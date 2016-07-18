/* Report that directory DIR was made, if OPTIONS requests this.  */
static void
announce_mkdir (char const *dir, void *options)
{
  struct cp_options const *x = options;
  if (x->verbose)
    prog_fprintf (stdout, _("creating directory %s"), quote (dir));
}

