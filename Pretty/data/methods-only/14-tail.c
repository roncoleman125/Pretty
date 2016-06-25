/* Return true if any of the N_FILES files in F are live, i.e., have
   open file descriptors.  */

static bool
any_live_files (const struct File_spec *f, size_t n_files)
{
  size_t i;

  for (i = 0; i < n_files; i++)
    if (0 <= f[i].fd)
      return true;
  return false;
}

