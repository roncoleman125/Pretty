/* Copy file FROM onto file TO and give TO the appropriate
   attributes.
   Return true if successful.  */

static bool
install_file_in_file (const char *from, const char *to,
                      const struct cp_options *x)
{
  struct stat from_sb;
  if (x->preserve_timestamps && stat (from, &from_sb) != 0)
    {
      error (0, errno, _("cannot stat %s"), quote (from));
      return false;
    }
  if (! copy_file (from, to, x))
    return false;
  if (strip_files)
    strip (to);
  if (x->preserve_timestamps && (strip_files || ! S_ISREG (from_sb.st_mode))
      && ! change_timestamps (&from_sb, to))
    return false;
  return change_attributes (to);
}

