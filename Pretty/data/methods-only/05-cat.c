int
main (int argc, char **argv)
{
  /* Optimal size of i/o operations of output.  */
  size_t outsize;

  /* Optimal size of i/o operations of input.  */
  size_t insize;

  size_t page_size = getpagesize ();

  /* Pointer to the input buffer.  */
  char *inbuf;

  /* Pointer to the output buffer.  */
  char *outbuf;

  bool ok = true;
  int c;

  /* Index in argv to processed argument.  */
  int argind;

  /* Device number of the output (file or whatever).  */
  dev_t out_dev;

  /* I-node number of the output.  */
  ino_t out_ino;

  /* True if the output file should not be the same as any input file.  */
  bool check_redirection = true;

  /* Nonzero if we have ever read standard input.  */
  bool have_read_stdin = false;

  struct stat stat_buf;

  /* Variables that are set according to the specified options.  */
  bool number = false;
  bool number_nonblank = false;
  bool squeeze_blank = false;
  bool show_ends = false;
  bool show_nonprinting = false;
  bool show_tabs = false;
  int file_open_mode = O_RDONLY;

  static struct option const long_options[] =
  {
    {"number-nonblank", no_argument, NULL, 'b'},
    {"number", no_argument, NULL, 'n'},
    {"squeeze-blank", no_argument, NULL, 's'},
    {"show-nonprinting", no_argument, NULL, 'v'},
    {"show-ends", no_argument, NULL, 'E'},
    {"show-tabs", no_argument, NULL, 'T'},
    {"show-all", no_argument, NULL, 'A'},
    {GETOPT_HELP_OPTION_DECL},
    {GETOPT_VERSION_OPTION_DECL},
    {NULL, 0, NULL, 0}
  };

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  /* Arrange to close stdout if we exit via the
     case_GETOPT_HELP_CHAR or case_GETOPT_VERSION_CHAR code.
     Normally STDOUT_FILENO is used rather than stdout, so
     close_stdout does nothing.  */
  atexit (close_stdout);

  /* Parse command line options.  */

  while ((c = getopt_long (argc, argv, "benstuvAET", long_options, NULL))
         != -1)
    {
      switch (c)
        {
        case 'b':
          number = true;
          number_nonblank = true;
          break;

        case 'e':
          show_ends = true;
          show_nonprinting = true;
          break;

        case 'n':
          number = true;
          break;

        case 's':
          squeeze_blank = true;
          break;

        case 't':
          show_tabs = true;
          show_nonprinting = true;
          break;

        case 'u':
          /* We provide the -u feature unconditionally.  */
          break;

        case 'v':
          show_nonprinting = true;
          break;

        case 'A':
          show_nonprinting = true;
          show_ends = true;
          show_tabs = true;
          break;

        case 'E':
          show_ends = true;
          break;

        case 'T':
          show_tabs = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  /* Get device, i-node number, and optimal blocksize of output.  */

  if (fstat (STDOUT_FILENO, &stat_buf) < 0)
    error (EXIT_FAILURE, errno, _("standard output"));

  outsize = io_blksize (stat_buf);
  /* Input file can be output file for non-regular files.
     fstat on pipes returns S_IFSOCK on some systems, S_IFIFO
     on others, so the checking should not be done for those types,
     and to allow things like cat < /dev/tty > /dev/tty, checking
     is not done for device files either.  */

  if (S_ISREG (stat_buf.st_mode))
    {
      out_dev = stat_buf.st_dev;
      out_ino = stat_buf.st_ino;
    }
  else
    {
      check_redirection = false;
      out_dev = 0;
      out_ino = 0;
    }

  if (! (number || show_ends || squeeze_blank))
    {
      file_open_mode |= O_BINARY;
      if (O_BINARY && ! isatty (STDOUT_FILENO))
        xfreopen (NULL, "wb", stdout);
    }

  /* Check if any of the input files are the same as the output file.  */

  /* Main loop.  */

  infile = "-";
  argind = optind;

  do
    {
      if (argind < argc)
        infile = argv[argind];

      if (STREQ (infile, "-"))
        {
          have_read_stdin = true;
          input_desc = STDIN_FILENO;
          if ((file_open_mode & O_BINARY) && ! isatty (STDIN_FILENO))
            xfreopen (NULL, "rb", stdin);
        }
      else
        {
          input_desc = open (infile, file_open_mode);
          if (input_desc < 0)
            {
              error (0, errno, "%s", infile);
              ok = false;
              continue;
            }
        }

      if (fstat (input_desc, &stat_buf) < 0)
        {
          error (0, errno, "%s", infile);
          ok = false;
          goto contin;
        }
      insize = io_blksize (stat_buf);

      fdadvise (input_desc, 0, 0, FADVISE_SEQUENTIAL);

      /* Compare the device and i-node numbers of this input file with
         the corresponding values of the (output file associated with)
         stdout, and skip this input file if they coincide.  Input
         files cannot be redirected to themselves.  */

      if (check_redirection
          && stat_buf.st_dev == out_dev && stat_buf.st_ino == out_ino
          && (input_desc != STDIN_FILENO))
        {
          error (0, 0, _("%s: input file is output file"), infile);
          ok = false;
          goto contin;
        }

      /* Select which version of `cat' to use.  If any format-oriented
         options were given use `cat'; otherwise use `simple_cat'.  */

      if (! (number || show_ends || show_nonprinting
             || show_tabs || squeeze_blank))
        {
          insize = MAX (insize, outsize);
          inbuf = xmalloc (insize + page_size - 1);

          ok &= simple_cat (ptr_align (inbuf, page_size), insize);
        }
      else
        {
          inbuf = xmalloc (insize + 1 + page_size - 1);

          /* Why are
             (OUTSIZE - 1 + INSIZE * 4 + LINE_COUNTER_BUF_LEN + PAGE_SIZE - 1)
             bytes allocated for the output buffer?

             A test whether output needs to be written is done when the input
             buffer empties or when a newline appears in the input.  After
             output is written, at most (OUTSIZE - 1) bytes will remain in the
             buffer.  Now INSIZE bytes of input is read.  Each input character
             may grow by a factor of 4 (by the prepending of M-^).  If all
             characters do, and no newlines appear in this block of input, we
             will have at most (OUTSIZE - 1 + INSIZE * 4) bytes in the buffer.
             If the last character in the preceding block of input was a
             newline, a line number may be written (according to the given
             options) as the first thing in the output buffer. (Done after the
             new input is read, but before processing of the input begins.)
             A line number requires seldom more than LINE_COUNTER_BUF_LEN
             positions.

             Align the output buffer to a page size boundary, for efficency on
             some paging implementations, so add PAGE_SIZE - 1 bytes to the
             request to make room for the alignment.  */

          outbuf = xmalloc (outsize - 1 + insize * 4 + LINE_COUNTER_BUF_LEN
                            + page_size - 1);

          ok &= cat (ptr_align (inbuf, page_size), insize,
                     ptr_align (outbuf, page_size), outsize, show_nonprinting,
                     show_tabs, number, number_nonblank, show_ends,
                     squeeze_blank);

          free (outbuf);
        }

      free (inbuf);

    contin:
      if (!STREQ (infile, "-") && close (input_desc) < 0)
        {
          error (0, errno, "%s", infile);
          ok = false;
        }
    }
  while (++argind < argc);

  if (have_read_stdin && close (STDIN_FILENO) < 0)
    error (EXIT_FAILURE, errno, _("closing standard input"));

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
