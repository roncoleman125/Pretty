/* Print all but the last N_ELIDE lines from the input available
   via file descriptor FD.  Return true upon success.
   Give a diagnostic and return false upon error.  */

/* NOTE: if the input file shrinks by more than N_ELIDE bytes between
   the length determination and the actual reading, then head fails.  */

static bool
elide_tail_bytes_file (const char *filename, int fd, uintmax_t n_elide)
{
  struct stat stats;

  if (presume_input_pipe || fstat (fd, &stats) || ! S_ISREG (stats.st_mode))
    {
      return elide_tail_bytes_pipe (filename, fd, n_elide);
    }
  else
    {
      off_t current_pos, end_pos;
      uintmax_t bytes_remaining;
      off_t diff;
      enum Copy_fd_status err;

      if ((current_pos = lseek (fd, (off_t) 0, SEEK_CUR)) == -1
          || (end_pos = lseek (fd, (off_t) 0, SEEK_END)) == -1)
        {
          error (0, errno, _("cannot lseek %s"), quote (filename));
          return false;
        }

      /* Be careful here.  The current position may actually be
         beyond the end of the file.  */
      bytes_remaining = (diff = end_pos - current_pos) < 0 ? 0 : diff;

      if (bytes_remaining <= n_elide)
        return true;

      /* Seek back to `current' position, then copy the required
         number of bytes from fd.  */
      if (lseek (fd, (off_t) 0, current_pos) == -1)
        {
          error (0, errno, _("%s: cannot lseek back to original position"),
                 quote (filename));
          return false;
        }

      err = copy_fd (fd, stdout, bytes_remaining - n_elide);
      if (err == COPY_FD_OK)
        return true;

      diagnose_copy_fd_failure (err, filename);
      return false;
    }
}

