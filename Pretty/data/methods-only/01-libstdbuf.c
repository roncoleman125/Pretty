static void
apply_mode (FILE *stream, const char *mode)
{
  char *buf = NULL;
  int setvbuf_mode;
  size_t size = 0;

  if (*mode == '0')
    setvbuf_mode = _IONBF;
  else if (*mode == 'L')
    setvbuf_mode = _IOLBF;      /* FIXME: should we allow 1ML  */
  else
    {
      setvbuf_mode = _IOFBF;
      verify (SIZE_MAX <= ULONG_MAX);
      size = strtoul (mode, NULL, 10);
      if (size > 0)
        {
          if (!(buf = malloc (size)))   /* will be freed by fclose()  */
            {
              /* We could defer the allocation to libc, however since
                 glibc currently ignores the combination of NULL buffer
                 with non zero size, we'll fail here.  */
              fprintf (stderr,
                       _("failed to allocate a %" PRIuMAX
                         " byte stdio buffer\n"), (uintmax_t) size);
              return;
            }
        }
      else
        {
          fprintf (stderr, _("invalid buffering mode %s for %s\n"),
                   mode, fileno_to_name (fileno (stream)));
          return;
        }
    }

  if (setvbuf (stream, buf, setvbuf_mode, size) != 0)
    {
      fprintf (stderr, _("could not set buffering of %s to mode %s\n"),
               fileno_to_name (fileno (stream)), mode);
      free (buf);
    }
}

__attribute__ ((constructor)) static void
stdbuf (void)
{
  char *e_mode = getenv ("_STDBUF_E");
  char *i_mode = getenv ("_STDBUF_I");
  char *o_mode = getenv ("_STDBUF_O");
  if (e_mode) /* Do first so can write errors to stderr  */
    apply_mode (stderr, e_mode);
  if (i_mode)
    apply_mode (stdin, i_mode);
  if (o_mode)
    apply_mode (stdout, o_mode);
}
