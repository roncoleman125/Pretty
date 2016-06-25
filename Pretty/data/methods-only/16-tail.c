/* Output the last N_BYTES bytes of file FILENAME open for reading in FD.
   Return true if successful.  */

static bool
tail_bytes (const char *pretty_filename, int fd, uintmax_t n_bytes,
            uintmax_t *read_pos)
{
  struct stat stats;

  if (fstat (fd, &stats))
    {
      error (0, errno, _("cannot fstat %s"), quote (pretty_filename));
      return false;
    }

  if (from_start)
    {
      if ( ! presume_input_pipe
           && S_ISREG (stats.st_mode) && n_bytes <= OFF_T_MAX)
        {
          xlseek (fd, n_bytes, SEEK_CUR, pretty_filename);
          *read_pos += n_bytes;
        }
      else
        {
          int t = start_bytes (pretty_filename, fd, n_bytes, read_pos);
          if (t)
            return t < 0;
        }
      *read_pos += dump_remainder (pretty_filename, fd, COPY_TO_EOF);
    }
  else
    {
      if ( ! presume_input_pipe
           && S_ISREG (stats.st_mode) && n_bytes <= OFF_T_MAX)
        {
          off_t current_pos = xlseek (fd, 0, SEEK_CUR, pretty_filename);
          off_t end_pos = xlseek (fd, 0, SEEK_END, pretty_filename);
          off_t diff = end_pos - current_pos;
          /* Be careful here.  The current position may actually be
             beyond the end of the file.  */
          off_t bytes_remaining = diff < 0 ? 0 : diff;
          off_t nb = n_bytes;

          if (bytes_remaining <= nb)
            {
              /* From the current position to end of file, there are no
                 more bytes than have been requested.  So reposition the
                 file pointer to the incoming current position and print
                 everything after that.  */
              *read_pos = xlseek (fd, current_pos, SEEK_SET, pretty_filename);
            }
          else
            {
              /* There are more bytes remaining than were requested.
                 Back up.  */
              *read_pos = xlseek (fd, -nb, SEEK_END, pretty_filename);
            }
          *read_pos += dump_remainder (pretty_filename, fd, n_bytes);
        }
      else
        return pipe_bytes (pretty_filename, fd, n_bytes, read_pos);
    }
  return true;
}

