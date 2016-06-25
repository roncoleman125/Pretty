/* Return true if ERROR_NUMBER is one of the values associated
   with a failed rmdir due to non-empty target directory.  */
static bool
errno_rmdir_non_empty (int error_number)
{
  return error_number == ENOTEMPTY || error_number == EEXIST;
}

