/* Mark as '.ignore'd each member of F that corresponds to a
   pipe or fifo, and return the number of non-ignored members.  */
static size_t
ignore_fifo_and_pipe (struct File_spec *f, size_t n_files)
{
  /* When there is no FILE operand and stdin is a pipe or FIFO
     POSIX requires that tail ignore the -f option.
     Since we allow multiple FILE operands, we extend that to say: with -f,
     ignore any "-" operand that corresponds to a pipe or FIFO.  */
  size_t n_viable = 0;

  size_t i;
  for (i = 0; i < n_files; i++)
    {
      bool is_a_fifo_or_pipe =
        (STREQ (f[i].name, "-")
         && !f[i].ignore
         && 0 <= f[i].fd
         && (S_ISFIFO (f[i].mode)
             || (HAVE_FIFO_PIPES != 1 && isapipe (f[i].fd))));
      if (is_a_fifo_or_pipe)
        f[i].ignore = true;
      else
        ++n_viable;
    }

  return n_viable;
}

int
main (int argc, char **argv)
{
  enum header_mode header_mode = multiple_files;
  bool ok = true;
  /* If from_start, the number of items to skip before printing; otherwise,
     the number of items at the end of the file to print.  Although the type
     is signed, the value is never negative.  */
  uintmax_t n_units = DEFAULT_N_LINES;
  size_t n_files;
  char **file;
  struct File_spec *F;
  size_t i;
  bool obsolete_option;

  /* The number of seconds to sleep between iterations.
     During one iteration, every file name or descriptor is checked to
     see if it has changed.  */
  double sleep_interval = 1.0;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;

  count_lines = true;
  forever = from_start = print_headers = false;
  obsolete_option = parse_obsolete_option (argc, argv, &n_units);
  argc -= obsolete_option;
  argv += obsolete_option;
  parse_options (argc, argv, &n_units, &header_mode, &sleep_interval);

  /* To start printing with item N_UNITS from the start of the file, skip
     N_UNITS - 1 items.  `tail -n +0' is actually meaningless, but for Unix
     compatibility it's treated the same as `tail -n +1'.  */
  if (from_start)
    {
      if (n_units)
        --n_units;
    }

  if (optind < argc)
    {
      n_files = argc - optind;
      file = argv + optind;
    }
  else
    {
      static char *dummy_stdin = (char *) "-";
      n_files = 1;
      file = &dummy_stdin;
    }

  {
    bool found_hyphen = false;

    for (i = 0; i < n_files; i++)
      if (STREQ (file[i], "-"))
        found_hyphen = true;

    /* When following by name, there must be a name.  */
    if (found_hyphen && follow_mode == Follow_name)
      error (EXIT_FAILURE, 0, _("cannot follow %s by name"), quote ("-"));

    /* When following forever, warn if any file is `-'.
       This is only a warning, since tail's output (before a failing seek,
       and that from any non-stdin files) might still be useful.  */
    if (forever && found_hyphen && isatty (STDIN_FILENO))
      error (0, 0, _("warning: following standard input"
                     " indefinitely is ineffective"));
  }

  F = xnmalloc (n_files, sizeof *F);
  for (i = 0; i < n_files; i++)
    F[i].name = file[i];

  if (header_mode == always
      || (header_mode == multiple_files && n_files > 1))
    print_headers = true;

  if (O_BINARY && ! isatty (STDOUT_FILENO))
    xfreopen (NULL, "wb", stdout);

  for (i = 0; i < n_files; i++)
    ok &= tail_file (&F[i], n_units);

  if (forever && ignore_fifo_and_pipe (F, n_files))
    {
      /* tailable_stdin() checks if the user specifies stdin via  "-",
         or implicitly by providing no arguments. If so, we won't use inotify.
         Technically, on systems with a working /dev/stdin, we *could*,
         but would it be worth it?  Verifying that it's a real device
         and hooked up to stdin is not trivial, while reverting to
         non-inotify-based tail_forever is easy and portable.

         any_remote_file() checks if the user has specified any
         files that reside on remote file systems.  inotify is not used
         in this case because it would miss any updates to the file
         that were not initiated from the local system.

         FIXME: inotify doesn't give any notification when a new
         (remote) file or directory is mounted on top a watched file.
         When follow_mode == Follow_name we would ideally like to detect that.
         Note if there is a change to the original file then we'll
         recheck it and follow the new file, or ignore it if the
         file has changed to being remote.

         FIXME: when using inotify, and a directory for a watched file
         is recreated, then we don't recheck any new file when
         follow_mode == Follow_name  */
      if (!disable_inotify && (tailable_stdin (F, n_files)
                               || any_remote_file (F, n_files)))
        disable_inotify = true;

      if (!disable_inotify)
        {
          int wd = inotify_init ();
          if (0 <= wd)
            {
              /* Flush any output from tail_file, now, since
                 tail_forever_inotify flushes only after writing,
                 not before reading.  */
              if (fflush (stdout) != 0)
                error (EXIT_FAILURE, errno, _("write error"));

              if (!tail_forever_inotify (wd, F, n_files, sleep_interval))
                exit (EXIT_FAILURE);
            }
          error (0, errno, _("inotify cannot be used, reverting to polling"));
        }
      disable_inotify = true;
      tail_forever (F, n_files, sleep_interval);
    }

  if (have_read_stdin && close (STDIN_FILENO) < 0)
    error (EXIT_FAILURE, errno, "-");
  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
