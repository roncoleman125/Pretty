/* FILE is the last operand of this command.
   Return true if FILE is a directory.
   But report an error and exit if there is a problem accessing FILE,
   or if FILE does not exist but would have to refer to an existing
   directory if it referred to anything at all.

   If the file exists, store the file's status into *ST.
   Otherwise, set *NEW_DST.  */

static bool
target_directory_operand (char const *file, struct stat *st, bool *new_dst)
{
  int err = (stat (file, st) == 0 ? 0 : errno);
  bool is_a_dir = !err && S_ISDIR (st->st_mode);
  if (err)
    {
      if (err != ENOENT)
        error (EXIT_FAILURE, err, _("accessing %s"), quote (file));
      *new_dst = true;
    }
  return is_a_dir;
}

