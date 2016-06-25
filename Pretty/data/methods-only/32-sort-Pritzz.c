/* Initialize BUF.  Reserve LINE_BYTES bytes for each line; LINE_BYTES
   must be at least sizeof (struct line).  Allocate ALLOC bytes
   initially.  */

static void
initbuf (struct buffer *buf, size_t line_bytes, size_t alloc)
{
  /* Ensure that the line array is properly aligned.  If the desired
     size cannot be allocated, repeatedly halve it until allocation
     succeeds.  The smaller allocation may hurt overall performance,
     but that's better than failing.  */
  while (true)
    {
      alloc += sizeof (struct line) - alloc % sizeof (struct line);
      buf->buf = malloc (alloc);
      if (buf->buf)
        break;
      alloc /= 2;
      if (alloc <= line_bytes + 1)
        xalloc_die ();
    }

  buf->line_bytes = line_bytes;
  buf->alloc = alloc;
  buf->used = buf->left = buf->nlines = 0;
  buf->eof = false;
}

