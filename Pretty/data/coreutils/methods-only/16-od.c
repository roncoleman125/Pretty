/* Read N bytes into BLOCK from the concatenation of the input files
   named in the global array FILE_LIST.  On the first call to this
   function, the global variable IN_STREAM is expected to be an open
   stream associated with the input file INPUT_FILENAME.  If all N
   bytes cannot be read from IN_STREAM, close IN_STREAM and update
   the global variables IN_STREAM and INPUT_FILENAME.  Then try to
   read the remaining bytes from the newly opened file.  Repeat if
   necessary until EOF is reached for the last file in FILE_LIST.
   On subsequent calls, don't modify BLOCK and return true.  Set
   *N_BYTES_IN_BUFFER to the number of bytes read.  If an error occurs,
   it will be detected through ferror when the stream is about to be
   closed.  If there is an error, give a message but continue reading
   as usual and return false.  Otherwise return true.  */

static bool
read_block (size_t n, char *block, size_t *n_bytes_in_buffer)
{
  bool ok = true;

  assert (0 < n && n <= bytes_per_block);

  *n_bytes_in_buffer = 0;

  if (n == 0)
    return true;

  while (in_stream != NULL)	/* EOF.  */
    {
      size_t n_needed;
      size_t n_read;

      n_needed = n - *n_bytes_in_buffer;
      n_read = fread (block + *n_bytes_in_buffer, 1, n_needed, in_stream);

      *n_bytes_in_buffer += n_read;

      if (n_read == n_needed)
        break;

      ok &= check_and_close (errno);

      ok &= open_next_file ();
    }

  return ok;
}

