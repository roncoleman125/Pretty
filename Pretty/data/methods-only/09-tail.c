/* Print the last N_LINES lines from the end of the standard input,
   open for reading as pipe FD.
   Buffer the text as a linked list of LBUFFERs, adding them as needed.
   Return true if successful.  */

static bool
pipe_lines (const char *pretty_filename, int fd, uintmax_t n_lines,
            uintmax_t *read_pos)
{
  struct linebuffer
  {
    char buffer[BUFSIZ];
    size_t nbytes;
    size_t nlines;
    struct linebuffer *next;
  };
  typedef struct linebuffer LBUFFER;
  LBUFFER *first, *last, *tmp;
  size_t total_lines = 0;	/* Total number of newlines in all buffers.  */
  bool ok = true;
  size_t n_read;		/* Size in bytes of most recent read */

  first = last = xmalloc (sizeof (LBUFFER));
  first->nbytes = first->nlines = 0;
  first->next = NULL;
  tmp = xmalloc (sizeof (LBUFFER));

  /* Input is always read into a fresh buffer.  */
  while (1)
    {
      n_read = safe_read (fd, tmp->buffer, BUFSIZ);
      if (n_read == 0 || n_read == SAFE_READ_ERROR)
        break;
      tmp->nbytes = n_read;
      *read_pos += n_read;
      tmp->nlines = 0;
      tmp->next = NULL;

      /* Count the number of newlines just read.  */
      {
        char const *buffer_end = tmp->buffer + n_read;
        char const *p = tmp->buffer;
        while ((p = memchr (p, '\n', buffer_end - p)))
          {
            ++p;
            ++tmp->nlines;
          }
      }
      total_lines += tmp->nlines;

      /* If there is enough room in the last buffer read, just append the new
         one to it.  This is because when reading from a pipe, `n_read' can
         often be very small.  */
      if (tmp->nbytes + last->nbytes < BUFSIZ)
        {
          memcpy (&last->buffer[last->nbytes], tmp->buffer, tmp->nbytes);
          last->nbytes += tmp->nbytes;
          last->nlines += tmp->nlines;
        }
      else
        {
          /* If there's not enough room, link the new buffer onto the end of
             the list, then either free up the oldest buffer for the next
             read if that would leave enough lines, or else malloc a new one.
             Some compaction mechanism is possible but probably not
             worthwhile.  */
          last = last->next = tmp;
          if (total_lines - first->nlines > n_lines)
            {
              tmp = first;
              total_lines -= first->nlines;
              first = first->next;
            }
          else
            tmp = xmalloc (sizeof (LBUFFER));
        }
    }

  free (tmp);

  if (n_read == SAFE_READ_ERROR)
    {
      error (0, errno, _("error reading %s"), quote (pretty_filename));
      ok = false;
      goto free_lbuffers;
    }

  /* If the file is empty, then bail out.  */
  if (last->nbytes == 0)
    goto free_lbuffers;

  /* This prevents a core dump when the pipe contains no newlines.  */
  if (n_lines == 0)
    goto free_lbuffers;

  /* Count the incomplete line on files that don't end with a newline.  */
  if (last->buffer[last->nbytes - 1] != '\n')
    {
      ++last->nlines;
      ++total_lines;
    }

  /* Run through the list, printing lines.  First, skip over unneeded
     buffers.  */
  for (tmp = first; total_lines - tmp->nlines > n_lines; tmp = tmp->next)
    total_lines -= tmp->nlines;

  /* Find the correct beginning, then print the rest of the file.  */
  {
    char const *beg = tmp->buffer;
    char const *buffer_end = tmp->buffer + tmp->nbytes;
    if (total_lines > n_lines)
      {
        /* Skip `total_lines' - `n_lines' newlines.  We made sure that
           `total_lines' - `n_lines' <= `tmp->nlines'.  */
        size_t j;
        for (j = total_lines - n_lines; j; --j)
          {
            beg = memchr (beg, '\n', buffer_end - beg);
            assert (beg);
            ++beg;
          }
      }

    xwrite_stdout (beg, buffer_end - beg);
  }

  for (tmp = tmp->next; tmp; tmp = tmp->next)
    xwrite_stdout (tmp->buffer, tmp->nbytes);

free_lbuffers:
  while (first)
    {
      tmp = first->next;
      free (first);
      first = tmp;
    }
  return ok;
}

