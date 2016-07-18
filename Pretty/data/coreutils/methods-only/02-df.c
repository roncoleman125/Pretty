/* Is FSTYPE a type of file system that should be omitted?  */

static bool
excluded_fstype (const char *fstype)
{
  const struct fs_type_list *fsp;

  if (fs_exclude_list == NULL || fstype == NULL)
    return false;
  for (fsp = fs_exclude_list; fsp; fsp = fsp->fs_next)
    if (STREQ (fstype, fsp->fs_name))
      return true;
  return false;
}

