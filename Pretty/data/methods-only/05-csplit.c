/* Read up to MAX_N_BYTES bytes from the input stream into DEST.
   Return the number of bytes read. */

static size_t
read_input (char *dest, size_t max_n_bytes)
{
  size_t bytes_read;

  if (max_n_bytes == 0)
    return 0;

  bytes_read = safe_read (STDIN_FILENO, dest, max_n_bytes);

  if (bytes_read == 0)
    have_read_eof = true;

  if (bytes_read == SAFE_READ_ERROR)
    {
      error (0, errno, _("read error"));
      cleanup_fatal ();
    }

  return bytes_read;
}

