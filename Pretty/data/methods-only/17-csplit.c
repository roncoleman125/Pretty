/* Search the buffers for line LINENUM, reading more input if necessary.
   Return a pointer to the line, or NULL if it is not found in the file. */

static struct cstring *
find_line (uintmax_t linenum)
{
  struct buffer_record *b;

  if (head == NULL && !load_buffer ())
    return NULL;

  if (linenum < head->start_line)
    return NULL;

  for (b = head;;)
    {
      if (linenum < b->start_line + b->num_lines)
        {
          /* The line is in this buffer. */
          struct line *l;
          size_t offset;	/* How far into the buffer the line is. */

          l = b->line_start;
          offset = linenum - b->start_line;
          /* Find the control record. */
          while (offset >= CTRL_SIZE)
            {
              l = l->next;
              offset -= CTRL_SIZE;
            }
          return &l->starts[offset];
        }
      if (b->next == NULL && !load_buffer ())
        return NULL;
      b = b->next;		/* Try the next data block. */
    }
}

