/* Turn on or off direct I/O mode for file descriptor FD, if possible.
   Try to turn it on if ENABLE is true.  Otherwise, try to turn it off.  */
static void
direct_mode (int fd, bool enable)
{
  if (O_DIRECT)
    {
      int fd_flags = fcntl (fd, F_GETFL);
      if (0 < fd_flags)
        {
          int new_flags = (enable
                           ? (fd_flags | O_DIRECT)
                           : (fd_flags & ~O_DIRECT));
          if (new_flags != fd_flags)
            fcntl (fd, F_SETFL, new_flags);
        }
    }

  /* This is Solaris-specific.  See the following for details:
     http://docs.sun.com/db/doc/816-0213/6m6ne37so?q=directio&a=view  */
  directio (fd, enable ? DIRECTIO_ON : DIRECTIO_OFF);
}

