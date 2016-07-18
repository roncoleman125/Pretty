/* Return a new buffer with room to store SIZE bytes, plus
   an extra byte for safety. */

static struct buffer_record *
create_new_buffer (size_t size)
{
  struct buffer_record *new_buffer = xmalloc (sizeof *new_buffer);

  new_buffer->buffer = xmalloc (size + 1);

  new_buffer->bytes_alloc = size;
  new_buffer->line_start = new_buffer->curr_line = NULL;

  return new_buffer;
}

