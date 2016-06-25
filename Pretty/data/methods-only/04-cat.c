/* Cat the file behind INPUT_DESC to the file behind OUTPUT_DESC.
   Return true if successful.
   Called if any option more than -u was specified.

   A newline character is always put at the end of the buffer, to make
   an explicit test for buffer end unnecessary.  */

static bool
cat (
     /* Pointer to the beginning of the input buffer.  */
     char *inbuf,

     /* Number of characters read in each read call.  */
     size_t insize,

     /* Pointer to the beginning of the output buffer.  */
     char *outbuf,

     /* Number of characters written by each write call.  */
     size_t outsize,

     /* Variables that have values according to the specified options.  */
     bool show_nonprinting,
     bool show_tabs,
     bool number,
     bool number_nonblank,
     bool show_ends,
     bool squeeze_blank)
{
  /* Last character read from the input buffer.  */
  unsigned char ch;

  /* Pointer to the next character in the input buffer.  */
  char *bpin;

  /* Pointer to the first non-valid byte in the input buffer, i.e. the
     current end of the buffer.  */
  char *eob;

  /* Pointer to the position where the next character shall be written.  */
  char *bpout;

  /* Number of characters read by the last read call.  */
  size_t n_read;

  /* Determines how many consecutive newlines there have been in the
     input.  0 newlines makes NEWLINES -1, 1 newline makes NEWLINES 1,
     etc.  Initially 0 to indicate that we are at the beginning of a
     new line.  The "state" of the procedure is determined by
     NEWLINES.  */
  int newlines = newlines2;

  /* If nonzero, use the FIONREAD ioctl, as an optimization.
     (On Ultrix, it is not supported on NFS file systems.)  */
  bool use_fionread = true;

  /* The inbuf pointers are initialized so that BPIN > EOB, and thereby input
     is read immediately.  */

  eob = inbuf;
  bpin = eob + 1;

  bpout = outbuf;

  while (true)
    {
      do
        {
          /* Write if there are at least OUTSIZE bytes in OUTBUF.  */

          if (outbuf + outsize <= bpout)
            {
              char *wp = outbuf;
              size_t remaining_bytes;
              do
                {
                  if (full_write (STDOUT_FILENO, wp, outsize) != outsize)
                    error (EXIT_FAILURE, errno, _("write error"));
                  wp += outsize;
                  remaining_bytes = bpout - wp;
                }
              while (outsize <= remaining_bytes);

              /* Move the remaining bytes to the beginning of the
                 buffer.  */

              memmove (outbuf, wp, remaining_bytes);
              bpout = outbuf + remaining_bytes;
            }

          /* Is INBUF empty?  */

          if (bpin > eob)
            {
              bool input_pending = false;
              int n_to_read = 0;

              /* Is there any input to read immediately?
                 If not, we are about to wait,
                 so write all buffered output before waiting.  */

              if (use_fionread
                  && ioctl (input_desc, FIONREAD, &n_to_read) < 0)
                {
                  /* Ultrix returns EOPNOTSUPP on NFS;
                     HP-UX returns ENOTTY on pipes.
                     SunOS returns EINVAL and
                     More/BSD returns ENODEV on special files
                     like /dev/null.
                     Irix-5 returns ENOSYS on pipes.  */
                  if (errno == EOPNOTSUPP || errno == ENOTTY
                      || errno == EINVAL || errno == ENODEV
                      || errno == ENOSYS)
                    use_fionread = false;
                  else
                    {
                      error (0, errno, _("cannot do ioctl on %s"),
                             quote (infile));
                      newlines2 = newlines;
                      return false;
                    }
                }
              if (n_to_read != 0)
                input_pending = true;

              if (!input_pending)
                write_pending (outbuf, &bpout);

              /* Read more input into INBUF.  */

              n_read = safe_read (input_desc, inbuf, insize);
              if (n_read == SAFE_READ_ERROR)
                {
                  error (0, errno, "%s", infile);
                  write_pending (outbuf, &bpout);
                  newlines2 = newlines;
                  return false;
                }
              if (n_read == 0)
                {
                  write_pending (outbuf, &bpout);
                  newlines2 = newlines;
                  return true;
                }

              /* Update the pointers and insert a sentinel at the buffer
                 end.  */

              bpin = inbuf;
              eob = bpin + n_read;
              *eob = '\n';
            }
          else
            {
              /* It was a real (not a sentinel) newline.  */

              /* Was the last line empty?
                 (i.e. have two or more consecutive newlines been read?)  */

              if (++newlines > 0)
                {
                  if (newlines >= 2)
                    {
                      /* Limit this to 2 here.  Otherwise, with lots of
                         consecutive newlines, the counter could wrap
                         around at INT_MAX.  */
                      newlines = 2;

                      /* Are multiple adjacent empty lines to be substituted
                         by single ditto (-s), and this was the second empty
                         line?  */
                      if (squeeze_blank)
                        {
                          ch = *bpin++;
                          continue;
                        }
                    }

                  /* Are line numbers to be written at empty lines (-n)?  */

                  if (number && !number_nonblank)
                    {
                      next_line_num ();
                      bpout = stpcpy (bpout, line_num_print);
                    }
                }

              /* Output a currency symbol if requested (-e).  */

              if (show_ends)
                *bpout++ = '$';

              /* Output the newline.  */

              *bpout++ = '\n';
            }
          ch = *bpin++;
        }
      while (ch == '\n');

      /* Are we at the beginning of a line, and line numbers are requested?  */

      if (newlines >= 0 && number)
        {
          next_line_num ();
          bpout = stpcpy (bpout, line_num_print);
        }

      /* Here CH cannot contain a newline character.  */

      /* The loops below continue until a newline character is found,
         which means that the buffer is empty or that a proper newline
         has been found.  */

      /* If quoting, i.e. at least one of -v, -e, or -t specified,
         scan for chars that need conversion.  */
      if (show_nonprinting)
        {
          while (true)
            {
              if (ch >= 32)
                {
                  if (ch < 127)
                    *bpout++ = ch;
                  else if (ch == 127)
                    {
                      *bpout++ = '^';
                      *bpout++ = '?';
                    }
                  else
                    {
                      *bpout++ = 'M';
                      *bpout++ = '-';
                      if (ch >= 128 + 32)
                        {
                          if (ch < 128 + 127)
                            *bpout++ = ch - 128;
                          else
                            {
                              *bpout++ = '^';
                              *bpout++ = '?';
                            }
                        }
                      else
                        {
                          *bpout++ = '^';
                          *bpout++ = ch - 128 + 64;
                        }
                    }
                }
              else if (ch == '\t' && !show_tabs)
                *bpout++ = '\t';
              else if (ch == '\n')
                {
                  newlines = -1;
                  break;
                }
              else
                {
                  *bpout++ = '^';
                  *bpout++ = ch + 64;
                }

              ch = *bpin++;
            }
        }
      else
        {
          /* Not quoting, neither of -v, -e, or -t specified.  */
          while (true)
            {
              if (ch == '\t' && show_tabs)
                {
                  *bpout++ = '^';
                  *bpout++ = ch + 64;
                }
              else if (ch != '\n')
                *bpout++ = ch;
              else
                {
                  newlines = -1;
                  break;
                }

              ch = *bpin++;
            }
        }
    }
}

