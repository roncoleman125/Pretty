/* Read from FD into the buffer BUF of size SIZE, processing any
   signals that arrive before bytes are read.  Return the number of
   bytes read if successful, -1 (setting errno) on failure.  */

static ssize_t
iread (int fd, char *buf, size_t size)
{
  while (true)
    {
      ssize_t nread;
      process_signals ();
      nread = read (fd, buf, size);
      if (! (nread < 0 && errno == EINTR))
        return nread;
    }
}

