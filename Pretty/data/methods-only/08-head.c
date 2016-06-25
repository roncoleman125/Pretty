/* Print all but the last N_ELIDE lines from the input available
   via file descriptor FD.  Return true upon success.
   Give a diagnostic and return nonzero upon error.  */

static bool
elide_tail_lines_file (const char *filename, int fd, uintmax_t n_elide)
{
  if (!presume_input_pipe)
    {
      /* Find the offset, OFF, of the Nth newline from the end,
         but not counting the last byte of the file.
         If found, write from current position to OFF, inclusive.
         Otherwise, just return true.  */

      off_t start_pos = lseek (fd, (off_t) 0, SEEK_CUR);
      off_t end_pos = lseek (fd, (off_t) 0, SEEK_END);
      if (0 <= start_pos && start_pos < end_pos)
        {
          /* If the file is empty, we're done.  */
          if (end_pos == 0)
            return true;

          return elide_tail_lines_seekable (filename, fd, n_elide,
                                            start_pos, end_pos);
        }

      /* lseek failed or the end offset precedes start.
         Fall through.  */
    }

  return elide_tail_lines_pipe (filename, fd, n_elide);
}

