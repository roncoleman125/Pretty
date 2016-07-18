/* Scan the buffer in B for newline characters
   and record the line start locations and lengths in B.
   Return the number of lines found in this buffer.

   There may be an incomplete line at the end of the buffer;
   a pointer is kept to this area, which will be used when
   the next buffer is filled. */

static size_t
record_line_starts (struct buffer_record *b)
{
  char *line_start;		/* Start of current line. */
  char *line_end;		/* End of each line found. */
  size_t bytes_left;		/* Length of incomplete last line. */
  size_t lines;			/* Number of lines found. */
  size_t line_length;		/* Length of each line found. */

  if (b->bytes_used == 0)
    return 0;

  lines = 0;
  line_start = b->buffer;
  bytes_left = b->bytes_used;

  while (true)
    {
      line_end = memchr (line_start, '\n', bytes_left);
      if (line_end == NULL)
        break;
      line_length = line_end - line_start + 1;
      keep_new_line (b, line_start, line_length);
      bytes_left -= line_length;
      line_start = line_end + 1;
      lines++;
    }

  /* Check for an incomplete last line. */
  if (bytes_left)
    {
      if (have_read_eof)
        {
          keep_new_line (b, line_start, bytes_left);
          lines++;
        }
      else
        save_to_hold_area (xmemdup (line_start, bytes_left), bytes_left);
    }

  b->num_lines = lines;
  b->first_available = b->start_line = last_line_number + 1;
  last_line_number += lines;

  return lines;
}

