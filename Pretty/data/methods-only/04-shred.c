/* Request that all data for FD be transferred to the corresponding
   storage device.  QNAME is the file name (quoted for colons).
   Report any errors found.  Return 0 on success, -1
   (setting errno) on failure.  It is not an error if fdatasync and/or
   fsync is not supported for this file, or if the file is not a
   writable file descriptor.  */
static int
dosync (int fd, char const *qname)
{
  int err;

  if (fdatasync (fd) == 0)
    return 0;
  err = errno;
  if ( ! ignorable_sync_errno (err))
    {
      error (0, err, _("%s: fdatasync failed"), qname);
      errno = err;
      return -1;
    }

  if (fsync (fd) == 0)
    return 0;
  err = errno;
  if ( ! ignorable_sync_errno (err))
    {
      error (0, err, _("%s: fsync failed"), qname);
      errno = err;
      return -1;
    }

  sync ();
  return 0;
}

