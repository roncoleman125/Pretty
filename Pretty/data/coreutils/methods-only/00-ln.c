/* FILE is the last operand of this command.  Return true if FILE is a
   directory.  But report an error there is a problem accessing FILE,
   or if FILE does not exist but would have to refer to an existing
   directory if it referred to anything at all.  */

static bool
target_directory_operand (char const *file)
{
  char const *b = last_component (file);
  size_t blen = strlen (b);
  bool looks_like_a_dir = (blen == 0 || ISSLASH (b[blen - 1]));
  struct stat st;
  int stat_result =
    (dereference_dest_dir_symlinks ? stat (file, &st) : lstat (file, &st));
  int err = (stat_result == 0 ? 0 : errno);
  bool is_a_dir = !err && S_ISDIR (st.st_mode);
  if (err && err != ENOENT)
    error (EXIT_FAILURE, err, _("accessing %s"), quote (file));
  if (is_a_dir < looks_like_a_dir)
    error (EXIT_FAILURE, err, _("target %s is not a directory"), quote (file));
  return is_a_dir;
}

