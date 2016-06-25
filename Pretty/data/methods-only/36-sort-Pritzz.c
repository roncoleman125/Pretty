/* Fill BUF reading from FP, moving buf->left bytes from the end
   of buf->buf to the beginning first.  If EOF is reached and the
   file wasn't terminated by a newline, supply one.  Set up BUF's line
   table too.  FILE is the name of the file corresponding to FP.
   Return true if some input was read.  */

static bool
fillbuf (struct buffer *buf, FILE *fp, char const *file)
{
  struct keyfield const *key = keylist;
  char eol = eolchar;
  size_t line_bytes = buf->line_bytes;
  size_t mergesize = merge_buffer_size - MIN_MERGE_BUFFER_SIZE;

  if (buf->eof)
    return false;

  if (buf->used != buf->left)
    {
      memmove (buf->buf, buf->buf + buf->used - buf->left, buf->left);
      buf->used = buf->left;
      buf->nlines = 0;
    }

  while (true)
    {
      char *ptr = buf->buf + buf->used;
      struct line *linelim = buffer_linelim (buf);
      struct line *line = linelim - buf->nlines;
      size_t avail = (char *) linelim - buf->nlines * line_bytes - ptr;
      char *line_start = buf->nlines ? line->text + line->length : buf->buf;

      while (line_bytes + 1 < avail)
        {
          /* Read as many bytes as possible, but do not read so many
             bytes that there might not be enough room for the
             corresponding line array.  The worst case is when the
             rest of the input file consists entirely of newlines,
             except that the last byte is not a newline.  */
          size_t readsize = (avail - 1) / (line_bytes + 1);
          size_t bytes_read = fread (ptr, 1, readsize, fp);
          char *ptrlim = ptr + bytes_read;
          char *p;
          avail -= bytes_read;

          if (bytes_read != readsize)
            {
              if (ferror (fp))
                die (_("read failed"), file);
              if (feof (fp))
                {
                  buf->eof = true;
                  if (buf->buf == ptrlim)
                    return false;
                  if (line_start != ptrlim && ptrlim[-1] != eol)
                    *ptrlim++ = eol;
                }
            }

          /* Find and record each line in the just-read input.  */
          while ((p = memchr (ptr, eol, ptrlim - ptr)))
            {
              /* Delimit the line with NUL. This eliminates the need to
                 temporarily replace the last byte with NUL when calling
                 xmemcoll(), which increases performance.  */
              *p = '\0';
              ptr = p + 1;
              line--;
              line->text = line_start;
              line->length = ptr - line_start;
              mergesize = MAX (mergesize, line->length);
              avail -= line_bytes;

              if (key)
                {
                  /* Precompute the position of the first key for
                     efficiency.  */
                  line->keylim = (key->eword == SIZE_MAX
                                  ? p
                                  : limfield (line, key));

                  if (key->sword != SIZE_MAX)
                    line->keybeg = begfield (line, key);
                  else
                    {
                      if (key->skipsblanks)
                        while (blanks[to_uchar (*line_start)])
                          line_start++;
                      line->keybeg = line_start;
                    }
                }

              line_start = ptr;
            }

          ptr = ptrlim;
          if (buf->eof)
            break;
        }

      buf->used = ptr - buf->buf;
      buf->nlines = buffer_linelim (buf) - line;
      if (buf->nlines != 0)
        {
          buf->left = ptr - line_start;
          merge_buffer_size = mergesize + MIN_MERGE_BUFFER_SIZE;
          return true;
        }

      {
        /* The current input line is too long to fit in the buffer.
           Double the buffer size and try again, keeping it properly
           aligned.  */
        size_t line_alloc = buf->alloc / sizeof (struct line);
        buf->buf = x2nrealloc (buf->buf, &line_alloc, sizeof (struct line));
        buf->alloc = line_alloc * sizeof (struct line);
      }
    }
}

