/* Initialize header information.
   If DESC is non-negative, it is a file descriptor open to
   FILENAME for reading.  */

static void
init_header (char const *filename, int desc)
{
  char *buf = NULL;
  struct stat st;
  struct timespec t;
  int ns;
  struct tm *tm;

  /* If parallel files or standard input, use current date. */
  if (STREQ (filename, "-"))
    desc = -1;
  if (0 <= desc && fstat (desc, &st) == 0)
    t = get_stat_mtime (&st);
  else
    {
      static struct timespec timespec;
      if (! timespec.tv_sec)
        gettime (&timespec);
      t = timespec;
    }

  ns = t.tv_nsec;
  tm = localtime (&t.tv_sec);
  if (tm == NULL)
    {
      buf = xmalloc (INT_BUFSIZE_BOUND (long int)
                     + MAX (10, INT_BUFSIZE_BOUND (int)));
      sprintf (buf, "%ld.%09d", (long int) t.tv_sec, ns);
    }
  else
    {
      size_t bufsize = nstrftime (NULL, SIZE_MAX, date_format, tm, 0, ns) + 1;
      buf = xmalloc (bufsize);
      nstrftime (buf, bufsize, date_format, tm, 0, ns);
    }

  free (date_text);
  date_text = buf;
  file_text = custom_header ? custom_header : desc < 0 ? "" : filename;
  header_width_available = (chars_per_line
                            - mbswidth (date_text, 0)
                            - mbswidth (file_text, 0));
}

