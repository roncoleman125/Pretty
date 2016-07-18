/* FILE is the last operand of this command.  Return true if FILE is a
   directory.  But report an error if there is a problem accessing FILE, other
   than nonexistence (errno == ENOENT).  */

static bool
target_directory_operand (char const *file)
{
  struct stat st;
  int err = (stat (file, &st) == 0 ? 0 : errno);
  bool is_a_dir = !err && S_ISDIR (st.st_mode);
  if (err && err != ENOENT)
    error (EXIT_FAILURE, err, _("accessing %s"), quote (file));
  return is_a_dir;
}

