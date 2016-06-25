/* Prepend the name S of length S_LEN, to the growing file_name, P.  */
static void
file_name_prepend (struct file_name *p, char const *s, size_t s_len)
{
  size_t n_free = p->start - p->buf;
  if (n_free < 1 + s_len)
    {
      size_t half = p->n_alloc + 1 + s_len;
      /* Use xnmalloc+free rather than xnrealloc, since with the latter
         we'd end up copying the data twice: once via realloc, then again
         to align it with the end of the new buffer.  With xnmalloc, we
         copy it only once.  */
      char *q = xnmalloc (2, half);
      size_t n_used = p->n_alloc - n_free;
      p->start = q + 2 * half - n_used;
      memcpy (p->start, p->buf + n_free, n_used);
      free (p->buf);
      p->buf = q;
      p->n_alloc = 2 * half;
    }

  p->start -= 1 + s_len;
  p->start[0] = '/';
  memcpy (p->start + 1, s, s_len);
}

