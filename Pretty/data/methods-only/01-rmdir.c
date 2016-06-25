/* Return true if when rmdir fails with errno == ERROR_NUMBER
   the directory may be empty.  */
static bool
errno_may_be_empty (int error_number)
{
  switch (error_number)
    {
    case EACCES:
    case EPERM:
    case EROFS:
    case EEXIST:
    case EBUSY:
      return true;
    default:
      return false;
    }
}

