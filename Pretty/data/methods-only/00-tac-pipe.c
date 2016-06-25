static bool
buf_init_from_stdin (Buf *x, char eol_byte)
{
  bool last_byte_is_eol_byte = true;
  bool ok = true;

  obstack_init (OBS);

  while (1)
    {
      char *buf = (char *) malloc (BUFFER_SIZE);
      size_t bytes_read;

      if (buf == NULL)
        {
          /* Fall back on the code that relies on a temporary file.
             Write all buffers to that file and free them.  */
          /* FIXME */
          ok = false;
          break;
        }
      bytes_read = full_read (STDIN_FILENO, buf, BUFFER_SIZE);
      if (bytes_read != buffer_size && errno != 0)
        error (EXIT_FAILURE, errno, _("read error"));

      {
        struct B_pair bp;
        bp.start = buf;
        bp.one_past_end = buf + bytes_read;
        obstack_grow (OBS, &bp, sizeof (bp));
      }

      if (bytes_read != 0)
        last_byte_is_eol_byte = (buf[bytes_read - 1] == eol_byte);

      if (bytes_read < BUFFER_SIZE)
        break;
    }

  if (ok)
    {
      /* If the file was non-empty and lacked an EOL_BYTE at its end,
         then add a buffer containing just that one byte.  */
      if (!last_byte_is_eol_byte)
        {
          char *buf = malloc (1);
          if (buf == NULL)
            {
              /* FIXME: just like above */
              ok = false;
            }
          else
            {
              struct B_pair bp;
              *buf = eol_byte;
              bp.start = buf;
              bp.one_past_end = buf + 1;
              obstack_grow (OBS, &bp, sizeof (bp));
            }
        }
    }

  x->n_bufs = obstack_object_size (OBS) / sizeof (x->p[0]);
  x->p = (struct B_pair *) obstack_finish (OBS);

  /* If there are two or more buffers and the last buffer is empty,
     then free the last one and decrement the buffer count.  */
  if (x->n_bufs >= 2
      && x->p[x->n_bufs - 1].start == x->p[x->n_bufs - 1].one_past_end)
    free (x->p[--(x->n_bufs)].start);

  return ok;
}

