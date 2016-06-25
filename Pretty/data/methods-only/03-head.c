/* Copy no more than N_BYTES from file descriptor SRC_FD to O_STREAM.
   Return an appropriate indication of success or failure. */

static enum Copy_fd_status
copy_fd (int src_fd, FILE *o_stream, uintmax_t n_bytes)
{
  char buf[BUFSIZ];
  const size_t buf_size = sizeof (buf);

  /* Copy the file contents.  */
  while (0 < n_bytes)
    {
      size_t n_to_read = MIN (buf_size, n_bytes);
      size_t n_read = safe_read (src_fd, buf, n_to_read);
      if (n_read == SAFE_READ_ERROR)
        return COPY_FD_READ_ERROR;

      n_bytes -= n_read;

      if (n_read == 0 && n_bytes != 0)
        return COPY_FD_UNEXPECTED_EOF;

      if (fwrite (buf, 1, n_read, o_stream) < n_read)
        return COPY_FD_WRITE_ERROR;
    }

  return COPY_FD_OK;
}

