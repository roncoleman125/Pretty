/* Print all but the last N_ELIDE lines from the input stream
   open for reading via file descriptor FD.
   Buffer the specified number of lines as a linked list of LBUFFERs,
   adding them as needed.  Return true if successful.  */

static bool
elide_tail_lines_pipe (const char *filename, int fd, uintmax_t n_elide)
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

  /* Always read into a fresh buffer.
     Read, (producing no output) until we've accumulated at least
     n_elide newlines, or until EOF, whichever comes first.  */
  while (1)
    {
      n_read = safe_read (fd, tmp->buffer, BUFSIZ);
      if (n_read == 0 || n_read == SAFE_READ_ERROR)
        break;
      tmp->nbytes = n_read;
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
          if (n_elide < total_lines - first->nlines)
            {
              fwrite (first->buffer, 1, first->nbytes, stdout);
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
      error (0, errno, _("error reading %s"), quote (filename));
      ok = false;
      goto free_lbuffers;
    }

  /* If we read any bytes at all, count the incomplete line
     on files that don't end with a newline.  */
  if (last->nbytes && last->buffer[last->nbytes - 1] != '\n')
    {
      ++last->nlines;
      ++total_lines;
    }

  for (tmp = first; n_elide < total_lines - tmp->nlines; tmp = tmp->next)
    {
      fwrite (tmp->buffer, 1, tmp->nbytes, stdout);
      total_lines -= tmp->nlines;
    }

  /* Print the first `total_lines - n_elide' lines of tmp->buffer.  */
  if (n_elide < total_lines)
    {
      size_t n = total_lines - n_elide;
      char const *buffer_end = tmp->buffer + tmp->nbytes;
      char const *p = tmp->buffer;
      while (n && (p = memchr (p, '\n', buffer_end - p)))
        {
          ++p;
          ++tmp->nlines;
          --n;
        }
      fwrite (tmp->buffer, 1, p - tmp->buffer, stdout);
    }

free_lbuffers:
  while (first)
    {
      tmp = first->next;
      free (first);
      first = tmp;
    }
  return ok;
}

