/* Change the file mode bits of the file identified by DESC or NAME to MODE.
   Use DESC if DESC is valid and fchmod is available, NAME otherwise.  */

static int
fchmod_or_lchmod (int desc, char const *name, mode_t mode)
{
  if (0 <= desc)
    return fchmod (desc, mode);
  return lchmod (name, mode);
}

