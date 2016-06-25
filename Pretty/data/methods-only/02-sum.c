/* Calculate and print the checksum and the size in 512-byte blocks
   of file FILE, or of the standard input if FILE is "-".
   If PRINT_NAME is >0, print FILE next to the checksum and size.
   Return true if successful.  */

static bool
sysv_sum_file (const char *file, int print_name)
{
  int fd;
  unsigned char buf[8192];
  uintmax_t total_bytes = 0;
  char hbuf[LONGEST_HUMAN_READABLE + 1];
  int r;
  int checksum;

  /* The sum of all the input bytes, modulo (UINT_MAX + 1).  */
  unsigned int s = 0;

  bool is_stdin = STREQ (file, "-");

  if (is_stdin)
    {
      fd = STDIN_FILENO;
      have_read_stdin = true;
      if (O_BINARY && ! isatty (STDIN_FILENO))
        xfreopen (NULL, "rb", stdin);
    }
  else
    {
      fd = open (file, O_RDONLY | O_BINARY);
      if (fd == -1)
        {
          error (0, errno, "%s", file);
          return false;
        }
    }

  while (1)
    {
      size_t i;
      size_t bytes_read = safe_read (fd, buf, sizeof buf);

      if (bytes_read == 0)
        break;

      if (bytes_read == SAFE_READ_ERROR)
        {
          error (0, errno, "%s", file);
          if (!is_stdin)
            close (fd);
          return false;
        }

      for (i = 0; i < bytes_read; i++)
        s += buf[i];
      total_bytes += bytes_read;
    }

  if (!is_stdin && close (fd) != 0)
    {
      error (0, errno, "%s", file);
      return false;
    }

  r = (s & 0xffff) + ((s & 0xffffffff) >> 16);
  checksum = (r & 0xffff) + (r >> 16);

  printf ("%d %s", checksum,
          human_readable (total_bytes, hbuf, human_ceiling, 1, 512));
  if (print_name)
    printf (" %s", file);
  putchar ('\n');

  return true;
}

