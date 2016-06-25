/* If the file ends with a `hole' (i.e., if sparse_copy set wrote_hole_at_eof),
   call this function to record the length of the output file.  */
static bool
sparse_copy_finalize (int dest_fd, char const *dst_name)
{
  off_t len = lseek (dest_fd, 0, SEEK_CUR);
  if (0 <= len && ftruncate (dest_fd, len) < 0)
    {
      error (0, errno, _("truncating %s"), quote (dst_name));
      return false;
    }

  return true;
}

