/* When a function like unlink, rmdir, or fstatat fails with an errno
   value of ERRNUM, return true if the specified file system object
   is guaranteed not to exist;  otherwise, return false.  */
static inline bool
nonexistent_file_errno (int errnum)
{
  /* Do not include ELOOP here, since the specified file may indeed
     exist, but be (in)accessible only via too long a symlink chain.
     Likewise for ENAMETOOLONG, since rm -f ./././.../foo may fail
     if the "..." part expands to a long enough sequence of "./"s,
     even though ./foo does indeed exist.

     Another case to consider is when a particular name is invalid for
     a given file system.  In 2011, smbfs returns EINVAL, but the next
     revision of POSIX will require EILSEQ for that situation:
     http://austingroupbugs.net/view.php?id=293
  */

  switch (errnum)
    {
    case EILSEQ:
    case EINVAL:
    case ENOENT:
    case ENOTDIR:
      return true;
    default:
      return false;
    }
}

