/* Return a pointer to the logical first line in the buffer and make the
   next line the logical first line.
   Return NULL if there is no more input. */

static struct cstring *
remove_line (void)
{
  /* If non-NULL, this is the buffer for which the previous call
     returned the final line.  So now, presuming that line has been
     processed, we can free the buffer and reset this pointer.  */
  static struct buffer_record *prev_buf = NULL;

  struct cstring *line;		/* Return value. */
  struct line *l;		/* For convenience. */

  if (prev_buf)
    {
      free_buffer (prev_buf);
      free (prev_buf);
      prev_buf = NULL;
    }

  if (head == NULL && !load_buffer ())
    return NULL;

  if (current_line < head->first_available)
    current_line = head->first_available;

  ++(head->first_available);

  l = head->curr_line;

  line = &l->starts[l->retrieve_index];

  /* Advance index to next line. */
  if (++l->retrieve_index == l->used)
    {
      /* Go on to the next line record. */
      head->curr_line = l->next;
      if (head->curr_line == NULL || head->curr_line->used == 0)
        {
          /* Go on to the next data block.
             but first record the current one so we can free it
             once the line we're returning has been processed.  */
          prev_buf = head;
          head = head->next;
        }
    }

  return line;
}

