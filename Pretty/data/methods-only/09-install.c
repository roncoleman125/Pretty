/* Copy file FROM into directory TO_DIR, keeping its same name,
   and give the copy the appropriate attributes.
   Return true if successful.  */

static bool
install_file_in_dir (const char *from, const char *to_dir,
                     const struct cp_options *x)
{
  const char *from_base = last_component (from);
  char *to = file_name_concat (to_dir, from_base, NULL);
  bool ret = install_file_in_file (from, to, x);
  free (to);
  return ret;
}

