int
main (int argc, char **argv)
{
  bool got_size = false;
  bool errors = false;
  off_t size IF_LINT ( = 0);
  off_t rsize = -1;
  rel_mode_t rel_mode = rm_abs;
  mode_t omode;
  int c, fd = -1, oflags;
  char const *fname;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "cor:s:", longopts, NULL)) != -1)
    {
      switch (c)
        {
        case 'c':
          no_create = true;
          break;

        case 'o':
          block_mode = true;
          break;

        case 'r':
          ref_file = optarg;
          break;

        case 's':
          /* skip any whitespace */
          while (isspace (to_uchar (*optarg)))
            optarg++;
          switch (*optarg)
            {
            case '<':
              rel_mode = rm_max;
              optarg++;
              break;
            case '>':
              rel_mode = rm_min;
              optarg++;
              break;
            case '/':
              rel_mode = rm_rdn;
              optarg++;
              break;
            case '%':
              rel_mode = rm_rup;
              optarg++;
              break;
            }
          /* skip any whitespace */
          while (isspace (to_uchar (*optarg)))
            optarg++;
          if (*optarg == '+' || *optarg == '-')
            {
              if (rel_mode)
                {
                  error (0, 0, _("multiple relative modifiers specified"));
                  /* Note other combinations are flagged as invalid numbers */
                  usage (EXIT_FAILURE);
                }
              rel_mode = rm_rel;
            }
          if (parse_len (optarg, &size) == -1)
            error (EXIT_FAILURE, errno, _("invalid number %s"),
                   quote (optarg));
          /* Rounding to multiple of 0 is nonsensical */
          if ((rel_mode == rm_rup || rel_mode == rm_rdn) && size == 0)
            error (EXIT_FAILURE, 0, _("division by zero"));
          got_size = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  argv += optind;
  argc -= optind;

  /* must specify either size or reference file */
  if (!ref_file && !got_size)
    {
      error (0, 0, _("you must specify either %s or %s"),
             quote_n (0, "--size"), quote_n (1, "--reference"));
      usage (EXIT_FAILURE);
    }
  /* must specify a relative size with a reference file */
  if (ref_file && got_size && !rel_mode)
    {
      error (0, 0, _("you must specify a relative %s with %s"),
             quote_n (0, "--size"), quote_n (1, "--reference"));
      usage (EXIT_FAILURE);
    }
  /* block_mode without size is not valid */
  if (block_mode && !got_size)
    {
      error (0, 0, _("%s was specified but %s was not"),
             quote_n (0, "--io-blocks"), quote_n (1, "--size"));
      usage (EXIT_FAILURE);
    }
  /* must specify at least 1 file */
  if (argc < 1)
    {
      error (0, 0, _("missing file operand"));
      usage (EXIT_FAILURE);
    }

  if (ref_file)
    {
      /* FIXME: Maybe support getting size of block devices.  */
      struct stat sb;
      if (stat (ref_file, &sb) != 0)
        error (EXIT_FAILURE, errno, _("cannot stat %s"), quote (ref_file));
      if (!S_ISREG (sb.st_mode) && !S_TYPEISSHM (&sb))
        error (EXIT_FAILURE, 0, _("cannot get the size of %s"),
               quote (ref_file));
      if (!got_size)
        size = sb.st_size;
      else
        rsize = sb.st_size;
    }

  oflags = O_WRONLY | (no_create ? 0 : O_CREAT) | O_NONBLOCK;
  omode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  while ((fname = *argv++) != NULL)
    {
      if ((fd = open (fname, oflags, omode)) == -1)
        {
          /* `truncate -s0 -c no-such-file`  shouldn't gen error
             `truncate -s0 no-such-dir/file` should gen ENOENT error
             `truncate -s0 no-such-dir/` should gen EISDIR error
             `truncate -s0 .` should gen EISDIR error */
          if (!(no_create && errno == ENOENT))
            {
              error (0, errno, _("cannot open %s for writing"),
                     quote (fname));
              errors = true;
            }
          continue;
        }


      if (fd != -1)
        {
          errors |= !do_ftruncate (fd, fname, size, rsize, rel_mode);
          if (close (fd) != 0)
            {
              error (0, errno, _("closing %s"), quote (fname));
              errors = true;
            }
        }
    }

  return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
