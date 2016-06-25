/* Compare content of opened files using file descriptors A_FD and B_FD. Return
   true if files are equal. */
static bool
have_same_content (int a_fd, int b_fd)
{
  enum { CMP_BLOCK_SIZE = 4096 };
  static char a_buff[CMP_BLOCK_SIZE];
  static char b_buff[CMP_BLOCK_SIZE];

  size_t size;
  while (0 < (size = full_read (a_fd, a_buff, sizeof a_buff))) {
    if (size != full_read (b_fd, b_buff, sizeof b_buff))
      return false;

    if (memcmp (a_buff, b_buff, size) != 0)
      return false;
  }

  return size == 0;
}

