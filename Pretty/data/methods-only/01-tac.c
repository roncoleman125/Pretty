/* Print the characters from START to PAST_END - 1.
   If START is NULL, just flush the buffer. */

static void
output (const char *start, const char *past_end)
{
  static char buffer[WRITESIZE];
  static size_t bytes_in_buffer = 0;
  size_t bytes_to_add = past_end - start;
  size_t bytes_available = WRITESIZE - bytes_in_buffer;

  if (start == 0)
    {
      fwrite (buffer, 1, bytes_in_buffer, stdout);
      bytes_in_buffer = 0;
      return;
    }

  /* Write out as many full buffers as possible. */
  while (bytes_to_add >= bytes_available)
    {
      memcpy (buffer + bytes_in_buffer, start, bytes_available);
      bytes_to_add -= bytes_available;
      start += bytes_available;
      fwrite (buffer, 1, WRITESIZE, stdout);
      bytes_in_buffer = 0;
      bytes_available = WRITESIZE;
    }

  memcpy (buffer + bytes_in_buffer, start, bytes_to_add);
  bytes_in_buffer += bytes_to_add;
}

