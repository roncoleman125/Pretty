/* Return true if an rmdir failure with errno == error_number
   for DIR is ignorable.  */
static bool
ignorable_failure (int error_number, char const *dir)
{
  return (ignore_fail_on_non_empty
          && (errno_rmdir_non_empty (error_number)
              || (errno_may_be_empty (error_number)
                  && is_empty_dir (AT_FDCWD, dir))));
}

