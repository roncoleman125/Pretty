/* Skip N_LINES lines at the start of file or pipe FD, and print
   any extra characters that were read beyond that.
   Return 1 on error, 0 if ok, -1 if EOF.  */

static int
start_lines (const char *pretty_filename, int fd, uintmax_t n_lines,
             uintmax_t *read_pos)
{
  if (n_lines == 0)
    return 0;

  while (1)
    {
      char buffer[BUFSIZ];
      char *p = buffer;
      size_t bytes_read = safe_read (fd, buffer, BUFSIZ);
      char *buffer_end = buffer + bytes_read;
      if (bytes_read == 0) /* EOF */
        return -1;
      if (bytes_read == SAFE_READ_ERROR) /* error */
        {
          error (0, errno, _("error reading %s"), quote (pretty_filename));
          return 1;
        }

      *read_pos += bytes_read;

      while ((p = memchr (p, '\n', buffer_end - p)))
        {
          ++p;
          if (--n_lines == 0)
            {
              if (p < buffer_end)
                xwrite_stdout (p, buffer_end - p);
              return 0;
            }
        }
    }
}

/* Without inotify support, always return false.  Otherwise, return false
   when FD is open on a file known to reside on a local file system.
   If fstatfs fails, give a diagnostic and return true.
   If fstatfs cannot be called, return true.  */
static bool
fremote (int fd, const char *name)
{
  bool remote = true;           /* be conservative (poll by default).  */

  struct statfs buf;
  int err = fstatfs (fd, &buf);
  if (err != 0)
    {
      /* On at least linux-2.6.38, fstatfs fails with ENOSYS when FD
         is open on a pipe.  Treat that like a remote file.  */
      if (errno != ENOSYS)
        error (0, errno, _("cannot determine location of %s. "
                           "reverting to polling"), quote (name));
    }
  else
    {
      switch (buf.f_type)
        {
        case S_MAGIC_AFS:
        case S_MAGIC_CIFS:
        case S_MAGIC_CODA:
        case S_MAGIC_FUSEBLK:
        case S_MAGIC_FUSECTL:
        case S_MAGIC_GFS:
        case S_MAGIC_KAFS:
        case S_MAGIC_LUSTRE:
        case S_MAGIC_NCP:
        case S_MAGIC_NFS:
        case S_MAGIC_NFSD:
        case S_MAGIC_OCFS2:
        case S_MAGIC_SMB:
          break;
        default:
          remote = false;
        }
    }

  return remote;
}
/* Without inotify support, whether a file is remote is irrelevant.
   Always return "false" in that case.  */

