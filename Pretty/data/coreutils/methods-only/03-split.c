/* Create or truncate a file.  */

static int
create (const char* name)
{
  if (verbose)
    fprintf (stdout, _("creating file %s\n"), quote (name));
  return open (name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
               (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));
}

