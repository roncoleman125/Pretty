/* Split into pieces of exactly N_BYTES bytes.
   Use buffer BUF, whose size is BUFSIZE.  */

static void
bytes_split (uintmax_t n_bytes, char *buf, size_t bufsize, uintmax_t max_files)
{
  size_t n_read;
  bool new_file_flag = true;
  size_t to_read;
  uintmax_t to_write = n_bytes;
  char *bp_out;
  uintmax_t opened = 0;

  do
    {
      n_read = full_read (STDIN_FILENO, buf, bufsize);
      if (n_read < bufsize && errno)
        error (EXIT_FAILURE, errno, "%s", infile);
      bp_out = buf;
      to_read = n_read;
      while (true)
        {
          if (to_read < to_write)
            {
              if (to_read)	/* do not write 0 bytes! */
                {
                  cwrite (new_file_flag, bp_out, to_read);
                  opened += new_file_flag;
                  to_write -= to_read;
                  new_file_flag = false;
                }
              break;
            }
          else
            {
              size_t w = to_write;
              cwrite (new_file_flag, bp_out, w);
              opened += new_file_flag;
              new_file_flag = !max_files || (opened < max_files);
              bp_out += w;
              to_read -= w;
              to_write = n_bytes;
            }
        }
    }
  while (n_read == bufsize);

  /* Ensure NUMBER files are created, which truncates
     any existing files or notifies any consumers on fifos.
     FIXME: Should we do this before EXIT_FAILURE?  */
  while (opened++ < max_files)
    cwrite (true, NULL, 0);
}

