/* Return a new buffer of at least MINSIZE bytes.  If a buffer of at
   least that size is currently free, use it, otherwise create a new one. */

static struct buffer_record *
get_new_buffer (size_t min_size)
{
  struct buffer_record *new_buffer; /* Buffer to return. */
  size_t alloc_size;	/* Actual size that will be requested. */

  alloc_size = START_SIZE;
  if (alloc_size < min_size)
    {
      size_t s = min_size - alloc_size + INCR_SIZE - 1;
      alloc_size += s - s % INCR_SIZE;
    }

  new_buffer = create_new_buffer (alloc_size);

  new_buffer->num_lines = 0;
  new_buffer->bytes_used = 0;
  new_buffer->start_line = new_buffer->first_available = last_line_number + 1;
  new_buffer->next = NULL;

  return new_buffer;
}

