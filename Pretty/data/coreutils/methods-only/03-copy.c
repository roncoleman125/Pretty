/* Perform the O(1) btrfs clone operation, if possible.
   Upon success, return 0.  Otherwise, return -1 and set errno.  */
static inline int
clone_file (int dest_fd, int src_fd)
{
  return ioctl (dest_fd, BTRFS_IOC_CLONE, src_fd);
  (void) dest_fd;
  (void) src_fd;
  errno = ENOTSUP;
  return -1;
}

