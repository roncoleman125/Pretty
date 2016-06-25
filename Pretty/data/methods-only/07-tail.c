/* Call lseek with the specified arguments, where file descriptor FD
   corresponds to the file, FILENAME.
   Give a diagnostic and exit nonzero if lseek fails.
   Otherwise, return the resulting offset.  */

static off_t
xlseek (int fd, off_t offset, int whence, char const *filename)
{
  off_t new_offset = lseek (fd, offset, whence);
  char buf[INT_BUFSIZE_BOUND (offset)];
  char *s;

  if (0 <= new_offset)
    return new_offset;

  s = offtostr (offset, buf);
  switch (whence)
    {
    case SEEK_SET:
      error (0, errno, _("%s: cannot seek to offset %s"),
             filename, s);
      break;
    case SEEK_CUR:
      error (0, errno, _("%s: cannot seek to relative offset %s"),
             filename, s);
      break;
    case SEEK_END:
      error (0, errno, _("%s: cannot seek to end-relative offset %s"),
             filename, s);
      break;
    default:
      abort ();
    }

  exit (EXIT_FAILURE);
}

