int
main (int argc, char **argv)
{
  struct stat stat_buf;
  enum Split_type split_type = type_undef;
  size_t in_blk_size = 0;	/* optimal block size of input file device */
  char *buf;			/* file i/o buffer */
  size_t page_size = getpagesize ();
  uintmax_t k_units = 0;
  uintmax_t n_units;

  static char const multipliers[] = "bEGKkMmPTYZ0";
  int c;
  int digits_optind = 0;
  off_t file_size;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  /* Parse command line options.  */

  infile = bad_cast ("-");
  outbase = bad_cast ("x");

  while (true)
    {
      /* This is the argv-index of the option we will read next.  */
      int this_optind = optind ? optind : 1;
      char *slash;

      c = getopt_long (argc, argv, "0123456789C:a:b:del:n:u", longopts, NULL);
      if (c == -1)
        break;

      switch (c)
        {
        case 'a':
          {
            unsigned long tmp;
            if (xstrtoul (optarg, NULL, 10, &tmp, "") != LONGINT_OK
                || SIZE_MAX / sizeof (size_t) < tmp)
              {
                error (0, 0, _("%s: invalid suffix length"), optarg);
                usage (EXIT_FAILURE);
              }
            suffix_length = tmp;
          }
          break;

        case 'b':
          if (split_type != type_undef)
            FAIL_ONLY_ONE_WAY ();
          split_type = type_bytes;
          if (xstrtoumax (optarg, NULL, 10, &n_units, multipliers) != LONGINT_OK
              || n_units == 0)
            {
              error (0, 0, _("%s: invalid number of bytes"), optarg);
              usage (EXIT_FAILURE);
            }
          /* If input is a pipe, we could get more data than is possible
             to write to a single file, so indicate that immediately
             rather than having possibly future invocations fail.  */
          if (OFF_T_MAX < n_units)
            error (EXIT_FAILURE, EFBIG,
                   _("%s: invalid number of bytes"), optarg);

          break;

        case 'l':
          if (split_type != type_undef)
            FAIL_ONLY_ONE_WAY ();
          split_type = type_lines;
          if (xstrtoumax (optarg, NULL, 10, &n_units, "") != LONGINT_OK
              || n_units == 0)
            {
              error (0, 0, _("%s: invalid number of lines"), optarg);
              usage (EXIT_FAILURE);
            }
          break;

        case 'C':
          if (split_type != type_undef)
            FAIL_ONLY_ONE_WAY ();
          split_type = type_byteslines;
          if (xstrtoumax (optarg, NULL, 10, &n_units, multipliers) != LONGINT_OK
              || n_units == 0 || SIZE_MAX < n_units)
            {
              error (0, 0, _("%s: invalid number of bytes"), optarg);
              usage (EXIT_FAILURE);
            }
          if (OFF_T_MAX < n_units)
            error (EXIT_FAILURE, EFBIG,
                   _("%s: invalid number of bytes"), optarg);
          break;

        case 'n':
          if (split_type != type_undef)
            FAIL_ONLY_ONE_WAY ();
          /* skip any whitespace */
          while (isspace (to_uchar (*optarg)))
            optarg++;
          if (strncmp (optarg, "r/", 2) == 0)
            {
              split_type = type_rr;
              optarg += 2;
            }
          else if (strncmp (optarg, "l/", 2) == 0)
            {
              split_type = type_chunk_lines;
              optarg += 2;
            }
          else
            split_type = type_chunk_bytes;
          if ((slash = strchr (optarg, '/')))
            parse_chunk (&k_units, &n_units, slash);
          else if (xstrtoumax (optarg, NULL, 10, &n_units, "") != LONGINT_OK
                   || n_units == 0)
            error (EXIT_FAILURE, 0, _("%s: invalid number of chunks"), optarg);
          break;

        case 'u':
          unbuffered = true;
          break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          if (split_type == type_undef)
            {
              split_type = type_digits;
              n_units = 0;
            }
          if (split_type != type_undef && split_type != type_digits)
            FAIL_ONLY_ONE_WAY ();
          if (digits_optind != 0 && digits_optind != this_optind)
            n_units = 0;	/* More than one number given; ignore other. */
          digits_optind = this_optind;
          if (!DECIMAL_DIGIT_ACCUMULATE (n_units, c - '0', uintmax_t))
            {
              char buffer[INT_BUFSIZE_BOUND (uintmax_t)];
              error (EXIT_FAILURE, 0,
                     _("line count option -%s%c... is too large"),
                     umaxtostr (n_units, buffer), c);
            }
          break;

        case 'd':
          suffix_alphabet = "0123456789";
          break;

        case 'e':
          elide_empty_files = true;
          break;

        case IO_BLKSIZE_OPTION:
          {
            uintmax_t tmp_blk_size;
            if (xstrtoumax (optarg, NULL, 10, &tmp_blk_size,
                            multipliers) != LONGINT_OK
                || tmp_blk_size == 0 || SIZE_MAX - page_size < tmp_blk_size)
              error (0, 0, _("%s: invalid IO block size"), optarg);
            else
              in_blk_size = tmp_blk_size;
          }
          break;

        case VERBOSE_OPTION:
          verbose = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  /* Handle default case.  */
  if (split_type == type_undef)
    {
      split_type = type_lines;
      n_units = 1000;
    }

  if (n_units == 0)
    {
      error (0, 0, _("%s: invalid number of lines"), "0");
      usage (EXIT_FAILURE);
    }

  set_suffix_length (n_units, split_type);

  /* Get out the filename arguments.  */

  if (optind < argc)
    infile = argv[optind++];

  if (optind < argc)
    outbase = argv[optind++];

  if (optind < argc)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  /* Open the input file.  */
  if (! STREQ (infile, "-")
      && fd_reopen (STDIN_FILENO, infile, O_RDONLY, 0) < 0)
    error (EXIT_FAILURE, errno, _("cannot open %s for reading"),
           quote (infile));

  /* Binary I/O is safer when byte counts are used.  */
  if (O_BINARY && ! isatty (STDIN_FILENO))
    xfreopen (NULL, "rb", stdin);

  /* Get the optimal block size of input device and make a buffer.  */

  if (fstat (STDIN_FILENO, &stat_buf) != 0)
    error (EXIT_FAILURE, errno, "%s", infile);
  if (in_blk_size == 0)
    in_blk_size = io_blksize (stat_buf);
  file_size = stat_buf.st_size;

  if (split_type == type_chunk_bytes || split_type == type_chunk_lines)
    {
      off_t input_offset = lseek (STDIN_FILENO, 0, SEEK_CUR);
      if (input_offset < 0)
        error (EXIT_FAILURE, 0, _("%s: cannot determine file size"),
               quote (infile));
      file_size -= input_offset;
      /* Overflow, and sanity checking.  */
      if (OFF_T_MAX < n_units)
        {
          char buffer[INT_BUFSIZE_BOUND (uintmax_t)];
          error (EXIT_FAILURE, EFBIG, _("%s: invalid number of chunks"),
                 umaxtostr (n_units, buffer));
        }
      /* increase file_size to n_units here, so that we still process
         any input data, and create empty files for the rest.  */
      file_size = MAX (file_size, n_units);
    }

  buf = ptr_align (xmalloc (in_blk_size + 1 + page_size - 1), page_size);

  switch (split_type)
    {
    case type_digits:
    case type_lines:
      lines_split (n_units, buf, in_blk_size);
      break;

    case type_bytes:
      bytes_split (n_units, buf, in_blk_size, 0);
      break;

    case type_byteslines:
      line_bytes_split (n_units);
      break;

    case type_chunk_bytes:
      if (k_units == 0)
        bytes_split (file_size / n_units, buf, in_blk_size, n_units);
      else
        bytes_chunk_extract (k_units, n_units, buf, in_blk_size, file_size);
      break;

    case type_chunk_lines:
      lines_chunk_split (k_units, n_units, buf, in_blk_size, file_size);
      break;

    case type_rr:
      /* Note, this is like `sed -n ${k}~${n}p` when k > 0,
         but the functionality is provided for symmetry.  */
      lines_rr (k_units, n_units, buf, in_blk_size);
      break;

    default:
      abort ();
    }

  if (close (STDIN_FILENO) != 0)
    error (EXIT_FAILURE, errno, "%s", infile);
  if (output_desc >= 0 && close (output_desc) < 0)
    error (EXIT_FAILURE, errno, "%s", outfile);

  exit (EXIT_SUCCESS);
}
