/* Append buffer BUF to the linked list of buffers that contain
   some data yet to be processed. */

static void
save_buffer (struct buffer_record *buf)
{
  struct buffer_record *p;

  buf->next = NULL;
  buf->curr_line = buf->line_start;

  if (head == NULL)
    head = buf;
  else
    {
      for (p = head; p->next; p = p->next)
        /* Do nothing. */ ;
      p->next = buf;
    }
}

