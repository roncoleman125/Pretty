/* Print all but the last N_ELIDE lines from the input available via
   the non-seekable file descriptor FD.  Return true upon success.
   Give a diagnostic and return false upon error.  */
static bool
elide_tail_bytes_pipe (const char *filename, int fd, uintmax_t n_elide_0)
{
  size_t n_elide = n_elide_0;


  /* If we're eliding no more than this many bytes, then it's ok to allocate
     more memory in order to use a more time-efficient algorithm.
     FIXME: use a fraction of available memory instead, as in sort.
     FIXME: is this even worthwhile?  */

  "HEAD_TAIL_PIPE_BYTECOUNT_THRESHOLD must be at least 2 * READ_BUFSIZE"

  if (SIZE_MAX < n_elide_0 + READ_BUFSIZE)
    {
      char umax_buf[INT_BUFSIZE_BOUND (n_elide_0)];
      error (EXIT_FAILURE, 0, _("%s: number of bytes is too large"),
             umaxtostr (n_elide_0, umax_buf));
    }

  /* Two cases to consider...
     1) n_elide is small enough that we can afford to double-buffer:
        allocate 2 * (READ_BUFSIZE + n_elide) bytes
     2) n_elide is too big for that, so we allocate only
        (READ_BUFSIZE + n_elide) bytes

     FIXME: profile, to see if double-buffering is worthwhile

     CAUTION: do not fail (out of memory) when asked to elide
     a ridiculous amount, but when given only a small input.  */

  if (n_elide <= HEAD_TAIL_PIPE_BYTECOUNT_THRESHOLD)
    {
      bool ok = true;
      bool first = true;
      bool eof = false;
      size_t n_to_read = READ_BUFSIZE + n_elide;
      bool i;
      char *b[2];
      b[0] = xnmalloc (2, n_to_read);
      b[1] = b[0] + n_to_read;

      for (i = false; ! eof ; i = !i)
        {
          size_t n_read = full_read (fd, b[i], n_to_read);
          size_t delta = 0;
          if (n_read < n_to_read)
            {
              if (errno != 0)
                {
                  error (0, errno, _("error reading %s"), quote (filename));
                  ok = false;
                  break;
                }

              /* reached EOF */
              if (n_read <= n_elide)
                {
                  if (first)
                    {
                      /* The input is no larger than the number of bytes
                         to elide.  So there's nothing to output, and
                         we're done.  */
                    }
                  else
                    {
                      delta = n_elide - n_read;
                    }
                }
              eof = true;
            }

          /* Output any (but maybe just part of the) elided data from
             the previous round.  */
          if ( ! first)
            {
              /* Don't bother checking for errors here.
                 If there's a failure, the test of the following
                 fwrite or in close_stdout will catch it.  */
              fwrite (b[!i] + READ_BUFSIZE, 1, n_elide - delta, stdout);
            }
          first = false;

          if (n_elide < n_read
              && fwrite (b[i], 1, n_read - n_elide, stdout) < n_read - n_elide)
            {
              error (0, errno, _("write error"));
              ok = false;
              break;
            }
        }

      free (b[0]);
      return ok;
    }
  else
    {
      /* Read blocks of size READ_BUFSIZE, until we've read at least n_elide
         bytes.  Then, for each new buffer we read, also write an old one.  */

      bool ok = true;
      bool eof = false;
      size_t n_read;
      bool buffered_enough;
      size_t i, i_next;
      char **b;
      /* Round n_elide up to a multiple of READ_BUFSIZE.  */
      size_t rem = READ_BUFSIZE - (n_elide % READ_BUFSIZE);
      size_t n_elide_round = n_elide + rem;
      size_t n_bufs = n_elide_round / READ_BUFSIZE + 1;
      b = xcalloc (n_bufs, sizeof *b);

      buffered_enough = false;
      for (i = 0, i_next = 1; !eof; i = i_next, i_next = (i_next + 1) % n_bufs)
        {
          if (b[i] == NULL)
            b[i] = xmalloc (READ_BUFSIZE);
          n_read = full_read (fd, b[i], READ_BUFSIZE);
          if (n_read < READ_BUFSIZE)
            {
              if (errno != 0)
                {
                  error (0, errno, _("error reading %s"), quote (filename));
                  ok = false;
                  goto free_mem;
                }
              eof = true;
            }

          if (i + 1 == n_bufs)
            buffered_enough = true;

          if (buffered_enough)
            {
              if (fwrite (b[i_next], 1, n_read, stdout) < n_read)
                {
                  error (0, errno, _("write error"));
                  ok = false;
                  goto free_mem;
                }
            }
        }

      /* Output any remainder: rem bytes from b[i] + n_read.  */
      if (rem)
        {
          if (buffered_enough)
            {
              size_t n_bytes_left_in_b_i = READ_BUFSIZE - n_read;
              if (rem < n_bytes_left_in_b_i)
                {
                  fwrite (b[i] + n_read, 1, rem, stdout);
                }
              else
                {
                  fwrite (b[i] + n_read, 1, n_bytes_left_in_b_i, stdout);
                  fwrite (b[i_next], 1, rem - n_bytes_left_in_b_i, stdout);
                }
            }
          else if (i + 1 == n_bufs)
            {
              /* This happens when n_elide < file_size < n_elide_round.

                 |READ_BUF.|
                 |                      |  rem |
                 |---------!---------!---------!---------|
                 |---- n_elide ---------|
                 |                      | x |
                 |                   |y |
                 |---- file size -----------|
                 |                   |n_read|
                 |---- n_elide_round ----------|
               */
              size_t y = READ_BUFSIZE - rem;
              size_t x = n_read - y;
              fwrite (b[i_next], 1, x, stdout);
            }
        }

    free_mem:;
      for (i = 0; i < n_bufs; i++)
        free (b[i]);
      free (b);

      return ok;
    }
}

