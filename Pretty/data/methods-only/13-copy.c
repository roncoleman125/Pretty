/* Return true if FILE, with mode MODE, is writable in the sense of 'mv'.
   Always consider a symbolic link to be writable.  */
static bool
writable_destination (char const *file, mode_t mode)
{
  return (S_ISLNK (mode)
          || can_write_any_file ()
          || euidaccess (file, W_OK) == 0);
}

