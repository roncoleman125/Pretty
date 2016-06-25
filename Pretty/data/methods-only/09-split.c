/* -n K/N: Extract Kth of N chunks.  */

static void
bytes_chunk_extract (uintmax_t k, uintmax_t n, char *buf, size_t bufsize,
                     off_t file_size)
{
  off_t start;
  off_t end;

  assert (k && n && k <= n && n <= file_size);

  start = (k - 1) * (file_size / n);
  end = (k == n) ? file_size : k * (file_size / n);

  if (lseek (STDIN_FILENO, start, SEEK_CUR) < 0)
    error (EXIT_FAILURE, errno, "%s", infile);

  while (start < end)
    {
      size_t n_read = full_read (STDIN_FILENO, buf, bufsize);
      n_read = MIN (n_read, end - start);
      if (n_read < bufsize && errno)
        error (EXIT_FAILURE, errno, "%s", infile);
      else if (n_read == 0)
        break; /* eof.  */
      if (full_write (STDOUT_FILENO, buf, n_read) != n_read)
        error (EXIT_FAILURE, errno, "%s", quote ("-"));
      start += n_read;
    }
}

typedef struct of_info
{
  char *of_name;
  int ofd;
  FILE* ofile;
} of_t;

enum
{
  OFD_NEW = -1,
  OFD_APPEND = -2
};

