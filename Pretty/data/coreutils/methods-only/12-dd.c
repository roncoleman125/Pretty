/* Wrapper around iread function to accumulate full blocks.  */
static ssize_t
iread_fullblock (int fd, char *buf, size_t size)
{
  ssize_t nread = 0;

  while (0 < size)
    {
      ssize_t ncurr = iread (fd, buf, size);
      if (ncurr < 0)
        return ncurr;
      if (ncurr == 0)
        break;
      nread += ncurr;
      buf   += ncurr;
      size  -= ncurr;
    }

  return nread;
}

