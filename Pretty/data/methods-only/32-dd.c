int
main (int argc, char **argv)
{
  int i;
  int exit_status;
  off_t offset;

  install_signal_handlers ();

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  /* Arrange to close stdout if parse_long_options exits.  */
  atexit (maybe_close_stdout);

  page_size = getpagesize ();

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE, Version,
                      usage, AUTHORS, (char const *) NULL);
  close_stdout_required = false;

  if (getopt_long (argc, argv, "", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  /* Initialize translation table to identity translation. */
  for (i = 0; i < 256; i++)
    trans_table[i] = i;

  /* Decode arguments. */
  scanargs (argc, argv);

  apply_translations ();

  if (input_file == NULL)
    {
      input_file = _("standard input");
      set_fd_flags (STDIN_FILENO, input_flags, input_file);
    }
  else
    {
      if (fd_reopen (STDIN_FILENO, input_file, O_RDONLY | input_flags, 0) < 0)
        error (EXIT_FAILURE, errno, _("opening %s"), quote (input_file));
    }

  offset = lseek (STDIN_FILENO, 0, SEEK_CUR);
  input_seekable = (0 <= offset);
  input_offset = MAX (0, offset);
  input_seek_errno = errno;

  if (output_file == NULL)
    {
      output_file = _("standard output");
      set_fd_flags (STDOUT_FILENO, output_flags, output_file);
    }
  else
    {
      mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
      int opts
        = (output_flags
           | (conversions_mask & C_NOCREAT ? 0 : O_CREAT)
           | (conversions_mask & C_EXCL ? O_EXCL : 0)
           | (seek_records || (conversions_mask & C_NOTRUNC) ? 0 : O_TRUNC));

      /* Open the output file with *read* access only if we might
         need to read to satisfy a `seek=' request.  If we can't read
         the file, go ahead with write-only access; it might work.  */
      if ((! seek_records
           || fd_reopen (STDOUT_FILENO, output_file, O_RDWR | opts, perms) < 0)
          && (fd_reopen (STDOUT_FILENO, output_file, O_WRONLY | opts, perms)
              < 0))
        error (EXIT_FAILURE, errno, _("opening %s"), quote (output_file));

      if (seek_records != 0 && !(conversions_mask & C_NOTRUNC))
        {
          uintmax_t size = seek_records * output_blocksize;
          unsigned long int obs = output_blocksize;

          if (OFF_T_MAX / output_blocksize < seek_records)
            error (EXIT_FAILURE, 0,
                   _("offset too large: "
                     "cannot truncate to a length of seek=%"PRIuMAX""
                     " (%lu-byte) blocks"),
                   seek_records, obs);

          if (ftruncate (STDOUT_FILENO, size) != 0)
            {
              /* Complain only when ftruncate fails on a regular file, a
                 directory, or a shared memory object, as POSIX 1003.1-2004
                 specifies ftruncate's behavior only for these file types.
                 For example, do not complain when Linux kernel 2.4 ftruncate
                 fails on /dev/fd0.  */
              int ftruncate_errno = errno;
              struct stat stdout_stat;
              if (fstat (STDOUT_FILENO, &stdout_stat) != 0)
                error (EXIT_FAILURE, errno, _("cannot fstat %s"),
                       quote (output_file));
              if (S_ISREG (stdout_stat.st_mode)
                  || S_ISDIR (stdout_stat.st_mode)
                  || S_TYPEISSHM (&stdout_stat))
                error (EXIT_FAILURE, ftruncate_errno,
                       _("failed to truncate to %"PRIuMAX" bytes"
                         " in output file %s"),
                       size, quote (output_file));
            }
        }
    }

  start_time = gethrxtime ();

  exit_status = dd_copy ();

  quit (exit_status);
}
