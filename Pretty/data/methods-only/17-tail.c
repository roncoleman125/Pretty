/* Output the last N_LINES lines of file FILENAME open for reading in FD.
   Return true if successful.  */

static bool
tail_lines (const char *pretty_filename, int fd, uintmax_t n_lines,
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
      int t = start_lines (pretty_filename, fd, n_lines, read_pos);
      if (t)
        return t < 0;
      *read_pos += dump_remainder (pretty_filename, fd, COPY_TO_EOF);
    }
  else
    {
      off_t start_pos = -1;
      off_t end_pos;

      /* Use file_lines only if FD refers to a regular file for
         which lseek (... SEEK_END) works.  */
      if ( ! presume_input_pipe
           && S_ISREG (stats.st_mode)
           && (start_pos = lseek (fd, 0, SEEK_CUR)) != -1
           && start_pos < (end_pos = lseek (fd, 0, SEEK_END)))
        {
          *read_pos = end_pos;
          if (end_pos != 0
              && ! file_lines (pretty_filename, fd, n_lines,
                               start_pos, end_pos, read_pos))
            return false;
        }
      else
        {
          /* Under very unlikely circumstances, it is possible to reach
             this point after positioning the file pointer to end of file
             via the `lseek (...SEEK_END)' above.  In that case, reposition
             the file pointer back to start_pos before calling pipe_lines.  */
          if (start_pos != -1)
            xlseek (fd, start_pos, SEEK_SET, pretty_filename);

          return pipe_lines (pretty_filename, fd, n_lines, read_pos);
        }
    }
  return true;
}

