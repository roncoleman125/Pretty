/* Write to FD the buffer BUF of size SIZE, processing any signals
   that arrive.  Return the number of bytes written, setting errno if
   this is less than SIZE.  Keep trying if there are partial
   writes.  */

static size_t
iwrite (int fd, char const *buf, size_t size)
{
  size_t total_written = 0;

  if ((output_flags & O_DIRECT) && size < output_blocksize)
    {
      int old_flags = fcntl (STDOUT_FILENO, F_GETFL);
      if (fcntl (STDOUT_FILENO, F_SETFL, old_flags & ~O_DIRECT) != 0)
        error (0, errno, _("failed to turn off O_DIRECT: %s"),
               quote (output_file));

      /* Since we have just turned off O_DIRECT for the final write,
         here we try to preserve some of its semantics.  First, use
         posix_fadvise to tell the system not to pollute the buffer
         cache with this data.  Don't bother to diagnose lseek or
         posix_fadvise failure. */
      off_t off = lseek (STDOUT_FILENO, 0, SEEK_CUR);
      if (0 <= off)
        fdadvise (STDOUT_FILENO, off, 0, FADVISE_DONTNEED);

      /* Attempt to ensure that that final block is committed
         to disk as quickly as possible.  */
      conversions_mask |= C_FSYNC;
    }

  while (total_written < size)
    {
      ssize_t nwritten;
      process_signals ();
      nwritten = write (fd, buf + total_written, size - total_written);
      if (nwritten < 0)
        {
          if (errno != EINTR)
            break;
        }
      else if (nwritten == 0)
        {
          /* Some buggy drivers return 0 when one tries to write beyond
             a device's end.  (Example: Linux kernel 1.2.13 on /dev/fd0.)
             Set errno to ENOSPC so they get a sensible diagnostic.  */
          errno = ENOSPC;
          break;
        }
      else
        total_written += nwritten;
    }

  return total_written;
}

