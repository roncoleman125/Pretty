int
main (int argc, char **argv)
{
  const char *error_message;	/* Return value from re_compile_pattern. */
  int optc;
  bool ok;
  size_t half_buffer_size;

  /* Initializer for file_list if no file-arguments
     were specified on the command line.  */
  static char const *const default_file_list[] = {"-", NULL};
  char const *const *file;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  separator = "\n";
  sentinel_length = 1;
  separator_ends_record = true;

  while ((optc = getopt_long (argc, argv, "brs:", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'b':
          separator_ends_record = false;
          break;
        case 'r':
          sentinel_length = 0;
          break;
        case 's':
          separator = optarg;
          if (*separator == 0)
            error (EXIT_FAILURE, 0, _("separator cannot be empty"));
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (sentinel_length == 0)
    {
      compiled_separator.buffer = NULL;
      compiled_separator.allocated = 0;
      compiled_separator.fastmap = compiled_separator_fastmap;
      compiled_separator.translate = NULL;
      error_message = re_compile_pattern (separator, strlen (separator),
                                          &compiled_separator);
      if (error_message)
        error (EXIT_FAILURE, 0, "%s", error_message);
    }
  else
    match_length = sentinel_length = strlen (separator);

  read_size = INITIAL_READSIZE;
  while (sentinel_length >= read_size / 2)
    {
      if (SIZE_MAX / 2 < read_size)
        xalloc_die ();
      read_size *= 2;
    }
  half_buffer_size = read_size + sentinel_length + 1;
  G_buffer_size = 2 * half_buffer_size;
  if (! (read_size < half_buffer_size && half_buffer_size < G_buffer_size))
    xalloc_die ();
  G_buffer = xmalloc (G_buffer_size);
  if (sentinel_length)
    {
      strcpy (G_buffer, separator);
      G_buffer += sentinel_length;
    }
  else
    {
      ++G_buffer;
    }

  file = (optind < argc
          ? (char const *const *) &argv[optind]
          : default_file_list);

  if (O_BINARY && ! isatty (STDOUT_FILENO))
    xfreopen (NULL, "wb", stdout);

  {
    size_t i;
    ok = true;
    for (i = 0; file[i]; ++i)
      ok &= tac_file (file[i]);
  }

  /* Flush the output buffer. */
  output ((char *) NULL, (char *) NULL);

  if (have_read_stdin && close (STDIN_FILENO) < 0)
    {
      error (0, errno, "-");
      ok = false;
    }

  size_t offset = sentinel_length ? sentinel_length : 1;
  free (G_buffer - offset);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
