/* -n r/[K/]N: Divide file into N chunks in round robin fashion.
   When K == 0, we try to keep the files open in parallel.
   If we run out of file resources, then we revert
   to opening and closing each file for each line.  */

static void
lines_rr (uintmax_t k, uintmax_t n, char *buf, size_t bufsize)
{
  bool file_limit;
  size_t i_file;
  of_t *files IF_LINT (= NULL);
  uintmax_t line_no;

  if (k)
    line_no = 1;
  else
    {
      if (SIZE_MAX < n)
        error (exit_failure, 0, "%s", _("memory exhausted"));
      files = xnmalloc (n, sizeof *files);

      /* Generate output file names. */
      for (i_file = 0; i_file < n; i_file++)
        {
          next_file_name ();
          files[i_file].of_name = xstrdup (outfile);
          files[i_file].ofd = OFD_NEW;
          files[i_file].ofile = NULL;
        }
      i_file = 0;
      file_limit = false;
    }

  while (true)
    {
      char *bp = buf, *eob;
      /* Use safe_read() rather than full_read() here
         so that we process available data immediately.  */
      size_t n_read = safe_read (STDIN_FILENO, buf, bufsize);
      if (n_read == SAFE_READ_ERROR)
        error (EXIT_FAILURE, errno, "%s", infile);
      else if (n_read == 0)
        break; /* eof.  */
      eob = buf + n_read;

      while (bp != eob)
        {
          size_t to_write;
          bool next = false;

          /* Find end of line. */
          char *bp_out = memchr (bp, '\n', eob - bp);
          if (bp_out)
            {
              bp_out++;
              next = true;
            }
          else
            bp_out = eob;
          to_write = bp_out - bp;

          if (k)
            {
              if (line_no == k && unbuffered)
                {
                  if (full_write (STDOUT_FILENO, bp, to_write) != to_write)
                    error (EXIT_FAILURE, errno, "%s", _("write error"));
                }
              else if (line_no == k && fwrite (bp, to_write, 1, stdout) != 1)
                {
                  clearerr (stdout); /* To silence close_stdout().  */
                  error (EXIT_FAILURE, errno, "%s", _("write error"));
                }
              if (next)
                line_no = (line_no == n) ? 1 : line_no + 1;
            }
          else
            {
              /* Secure file descriptor. */
              file_limit |= ofile_open (files, i_file, n);
              if (unbuffered)
                {
                  /* Note writing to fd, rather than flushing the FILE gives
                     an 8% performance benefit, due to reduced data copying.  */
                  if (full_write (files[i_file].ofd, bp, to_write) != to_write)
                    error (EXIT_FAILURE, errno, "%s", files[i_file].of_name);
                }
              else if (fwrite (bp, to_write, 1, files[i_file].ofile) != 1)
                error (EXIT_FAILURE, errno, "%s", files[i_file].of_name);
              if (file_limit)
                {
                  if (fclose (files[i_file].ofile) != 0)
                    error (EXIT_FAILURE, errno, "%s", files[i_file].of_name);
                  files[i_file].ofd = OFD_APPEND;
                }
              if (next && ++i_file == n)
                i_file = 0;
            }

          bp = bp_out;
        }
    }

  /* Ensure all files created, so that any existing files are truncated,
     and to signal any waiting fifo consumers.
     Also, close any open file descriptors.
     FIXME: Should we do this before EXIT_FAILURE?  */
  for (i_file = 0; !k && !elide_empty_files && i_file < n; i_file++)
    {
      file_limit |= ofile_open (files, i_file, n);
      if (fclose (files[i_file].ofile) != 0)
        error (EXIT_FAILURE, errno, "%s", files[i_file].of_name);
    }
}

  do								\
    {								\
      error (0, 0, _("cannot split in more than one way"));	\
      usage (EXIT_FAILURE);					\
    }								\
  while (0)

/* Parse K/N syntax of chunk options.  */

static void
parse_chunk (uintmax_t *k_units, uintmax_t *n_units, char *slash)
{
  *slash = '\0';
  if (xstrtoumax (slash+1, NULL, 10, n_units, "") != LONGINT_OK
      || *n_units == 0)
    error (EXIT_FAILURE, 0, _("%s: invalid number of chunks"), slash+1);
  if (slash != optarg           /* a leading number is specified.  */
      && (xstrtoumax (optarg, NULL, 10, k_units, "") != LONGINT_OK
          || *k_units == 0 || *n_units < *k_units))
    error (EXIT_FAILURE, 0, _("%s: invalid chunk number"), optarg);
}


