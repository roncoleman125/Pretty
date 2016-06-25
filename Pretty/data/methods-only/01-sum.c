/* Calculate and print the rotated checksum and the size in 1K blocks
   of file FILE, or of the standard input if FILE is "-".
   If PRINT_NAME is >1, print FILE next to the checksum and size.
   The checksum varies depending on sizeof (int).
   Return true if successful.  */

static bool
bsd_sum_file (const char *file, int print_name)
{
  FILE *fp;
  int checksum = 0;	/* The checksum mod 2^16. */
  uintmax_t total_bytes = 0;	/* The number of bytes. */
  int ch;		/* Each character read. */
  char hbuf[LONGEST_HUMAN_READABLE + 1];
  bool is_stdin = STREQ (file, "-");

  if (is_stdin)
    {
      fp = stdin;
      have_read_stdin = true;
      if (O_BINARY && ! isatty (STDIN_FILENO))
        xfreopen (NULL, "rb", stdin);
    }
  else
    {
      fp = fopen (file, (O_BINARY ? "rb" : "r"));
      if (fp == NULL)
        {
          error (0, errno, "%s", file);
          return false;
        }
    }

  fadvise (fp, FADVISE_SEQUENTIAL);

  while ((ch = getc (fp)) != EOF)
    {
      total_bytes++;
      checksum = (checksum >> 1) + ((checksum & 1) << 15);
      checksum += ch;
      checksum &= 0xffff;	/* Keep it within bounds. */
    }

  if (ferror (fp))
    {
      error (0, errno, "%s", file);
      if (!is_stdin)
        fclose (fp);
      return false;
    }

  if (!is_stdin && fclose (fp) != 0)
    {
      error (0, errno, "%s", file);
      return false;
    }

  printf ("%05d %5s", checksum,
          human_readable (total_bytes, hbuf, human_ceiling, 1, 1024));
  if (print_name > 1)
    printf (" %s", file);
  putchar ('\n');

  return true;
}

