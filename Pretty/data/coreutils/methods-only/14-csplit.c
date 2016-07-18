/* Fill a buffer of input.

   Set the initial size of the buffer to a default.
   Fill the buffer (from the hold area and input stream)
   and find the individual lines.
   If no lines are found (the buffer is too small to hold the next line),
   release the current buffer (whose contents would have been put in the
   hold area) and repeat the process with another large buffer until at least
   one entire line has been read.

   Return true if a new buffer was obtained, otherwise false
   (in which case end-of-file must have been encountered). */

static bool
load_buffer (void)
{
  struct buffer_record *b;
  size_t bytes_wanted = START_SIZE; /* Minimum buffer size. */
  size_t bytes_avail;		/* Size of new buffer created. */
  size_t lines_found;		/* Number of lines in this new buffer. */
  char *p;			/* Place to load into buffer. */

  if (have_read_eof)
    return false;

  /* We must make the buffer at least as large as the amount of data
     in the partial line left over from the last call. */
  if (bytes_wanted < hold_count)
    bytes_wanted = hold_count;

  while (1)
    {
      b = get_new_buffer (bytes_wanted);
      bytes_avail = b->bytes_alloc; /* Size of buffer returned. */
      p = b->buffer;

      /* First check the `holding' area for a partial line. */
      if (hold_count)
        {
          memcpy (p, hold_area, hold_count);
          p += hold_count;
          b->bytes_used += hold_count;
          bytes_avail -= hold_count;
          hold_count = 0;
        }

      b->bytes_used += read_input (p, bytes_avail);

      lines_found = record_line_starts (b);
      if (!lines_found)
        free_buffer (b);

      if (lines_found || have_read_eof)
        break;

      if (xalloc_oversized (2, b->bytes_alloc))
        xalloc_die ();
      bytes_wanted = 2 * b->bytes_alloc;
      free_buffer (b);
      free (b);
    }

  if (lines_found)
    save_buffer (b);
  else
    free (b);

  return lines_found != 0;
}

