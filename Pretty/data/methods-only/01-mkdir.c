/* Report that directory DIR was made, if OPTIONS requests this.  */
static void
announce_mkdir (char const *dir, void *options)
{
  struct mkdir_options const *o = options;
  if (o->created_directory_format)
    prog_fprintf (stdout, o->created_directory_format, quote (dir));
}

