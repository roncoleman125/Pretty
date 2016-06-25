/* Print the last N_BYTES characters from the end of pipe FD.
   This is a stripped down version of pipe_lines.
   Return true if successful.  */

static bool
pipe_bytes (const char *pretty_filename, int fd, uintmax_t n_bytes,
            uintmax_t *read_pos)
{
  struct charbuffer
  {
    char buffer[BUFSIZ];
    size_t nbytes;
    struct charbuffer *next;
  };
  typedef struct charbuffer CBUFFER;
  CBUFFER *first, *last, *tmp;
  size_t i;			/* Index into buffers.  */
  size_t total_bytes = 0;	/* Total characters in all buffers.  */
  bool ok = true;
  size_t n_read;

  first = last = xmalloc (sizeof (CBUFFER));
  first->nbytes = 0;
  first->next = NULL;
  tmp = xmalloc (sizeof (CBUFFER));

  /* Input is always read into a fresh buffer.  */
  while (1)
    {
      n_read = safe_read (fd, tmp->buffer, BUFSIZ);
      if (n_read == 0 || n_read == SAFE_READ_ERROR)
        break;
      *read_pos += n_read;
      tmp->nbytes = n_read;
      tmp->next = NULL;

      total_bytes += tmp->nbytes;
      /* If there is enough room in the last buffer read, just append the new
         one to it.  This is because when reading from a pipe, `nbytes' can
         often be very small.  */
      if (tmp->nbytes + last->nbytes < BUFSIZ)
        {
          memcpy (&last->buffer[last->nbytes], tmp->buffer, tmp->nbytes);
          last->nbytes += tmp->nbytes;
        }
      else
        {
          /* If there's not enough room, link the new buffer onto the end of
             the list, then either free up the oldest buffer for the next
             read if that would leave enough characters, or else malloc a new
             one.  Some compaction mechanism is possible but probably not
             worthwhile.  */
          last = last->next = tmp;
          if (total_bytes - first->nbytes > n_bytes)
            {
              tmp = first;
              total_bytes -= first->nbytes;
              first = first->next;
            }
          else
            {
              tmp = xmalloc (sizeof (CBUFFER));
            }
        }
    }

  free (tmp);

  if (n_read == SAFE_READ_ERROR)
    {
      error (0, errno, _("error reading %s"), quote (pretty_filename));
      ok = false;
      goto free_cbuffers;
    }

  /* Run through the list, printing characters.  First, skip over unneeded
     buffers.  */
  for (tmp = first; total_bytes - tmp->nbytes > n_bytes; tmp = tmp->next)
    total_bytes -= tmp->nbytes;

  /* Find the correct beginning, then print the rest of the file.
     We made sure that `total_bytes' - `n_bytes' <= `tmp->nbytes'.  */
  if (total_bytes > n_bytes)
    i = total_bytes - n_bytes;
  else
    i = 0;
  xwrite_stdout (&tmp->buffer[i], tmp->nbytes - i);

  for (tmp = tmp->next; tmp; tmp = tmp->next)
    xwrite_stdout (tmp->buffer, tmp->nbytes);

free_cbuffers:
  while (first)
    {
      tmp = first->next;
      free (first);
      first = tmp;
    }
  return ok;
}

