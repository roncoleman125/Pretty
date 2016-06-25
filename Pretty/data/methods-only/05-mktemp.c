int
main (int argc, char **argv)
{
  char const *dest_dir;
  char const *dest_dir_arg = NULL;
  bool suppress_stderr = false;
  int c;
  unsigned int n_args;
  char *template;
  char *suffix = NULL;
  bool use_dest_dir = false;
  bool deprecated_t_option = false;
  bool create_directory = false;
  bool dry_run = false;
  int status = EXIT_SUCCESS;
  size_t x_count;
  size_t suffix_len;
  char *dest_name;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (maybe_close_stdout);

  while ((c = getopt_long (argc, argv, "dp:qtuV", longopts, NULL)) != -1)
    {
      switch (c)
        {
        case 'd':
          create_directory = true;
          break;
        case 'p':
          dest_dir_arg = optarg;
          use_dest_dir = true;
          break;
        case 'q':
          suppress_stderr = true;
          break;
        case 't':
          use_dest_dir = true;
          deprecated_t_option = true;
          break;
        case 'u':
          dry_run = true;
          break;

        case TMPDIR_OPTION:
          use_dest_dir = true;
          dest_dir_arg = optarg;
          break;

        case SUFFIX_OPTION:
          suffix = optarg;
          break;

        case_GETOPT_HELP_CHAR;

        case 'V': /* Undocumented alias.  FIXME: remove in 2011.  */
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  if (suppress_stderr)
    {
      /* From here on, redirect stderr to /dev/null.
         A diagnostic from getopt_long, above, would still go to stderr.  */
      if (!freopen ("/dev/null", "wb", stderr))
        error (EXIT_FAILURE, errno,
               _("failed to redirect stderr to /dev/null"));
    }

  n_args = argc - optind;
  if (2 <= n_args)
    {
      error (0, 0, _("too many templates"));
      usage (EXIT_FAILURE);
    }

  if (n_args == 0)
    {
      use_dest_dir = true;
      template = (char *) default_template;
    }
  else
    {
      template = argv[optind];
    }

  if (suffix)
    {
      size_t len = strlen (template);
      if (!len || template[len - 1] != 'X')
        {
          error (EXIT_FAILURE, 0,
                 _("with --suffix, template %s must end in X"),
                 quote (template));
        }
      suffix_len = strlen (suffix);
      dest_name = xcharalloc (len + suffix_len + 1);
      memcpy (dest_name, template, len);
      memcpy (dest_name + len, suffix, suffix_len + 1);
      template = dest_name;
      suffix = dest_name + len;
    }
  else
    {
      template = xstrdup (template);
      suffix = strrchr (template, 'X');
      if (!suffix)
        suffix = strchr (template, '\0');
      else
        suffix++;
      suffix_len = strlen (suffix);
    }

  /* At this point, template is malloc'd, and suffix points into template.  */
  if (suffix_len && last_component (suffix) != suffix)
    {
      error (EXIT_FAILURE, 0,
             _("invalid suffix %s, contains directory separator"),
             quote (suffix));
    }
  x_count = count_consecutive_X_s (template, suffix - template);
  if (x_count < 3)
    error (EXIT_FAILURE, 0, _("too few X's in template %s"), quote (template));

  if (use_dest_dir)
    {
      if (deprecated_t_option)
        {
          char *env = getenv ("TMPDIR");
          dest_dir = (env && *env
                      ? env
                      : (dest_dir_arg ? dest_dir_arg : "/tmp"));

          if (last_component (template) != template)
            error (EXIT_FAILURE, 0,
                   _("invalid template, %s, contains directory separator"),
                   quote (template));
        }
      else
        {
          if (dest_dir_arg && *dest_dir_arg)
            dest_dir = dest_dir_arg;
          else
            {
              char *env = getenv ("TMPDIR");
              dest_dir = (env && *env ? env : "/tmp");
            }
          if (IS_ABSOLUTE_FILE_NAME (template))
            error (EXIT_FAILURE, 0,
                   _("invalid template, %s; with --tmpdir,"
                     " it may not be absolute"),
                   quote (template));
        }

      dest_name = file_name_concat (dest_dir, template, NULL);
      free (template);
      template = dest_name;
      /* Note that suffix is now invalid.  */
    }

  /* Make a copy to be used in case of diagnostic, since failing
     mkstemp may leave the buffer in an undefined state.  */
  dest_name = xstrdup (template);

  if (create_directory)
    {
      int err = mkdtemp_len (dest_name, suffix_len, x_count, dry_run);
      if (err != 0)
        {
          error (0, errno, _("failed to create directory via template %s"),
                 quote (template));
          status = EXIT_FAILURE;
        }
    }
  else
    {
      int fd = mkstemp_len (dest_name, suffix_len, x_count, dry_run);
      if (fd < 0 || (!dry_run && close (fd) != 0))
        {
          error (0, errno, _("failed to create file via template %s"),
                 quote (template));
          status = EXIT_FAILURE;
        }
    }

  if (status == EXIT_SUCCESS)
    {
      puts (dest_name);
      /* If we created a file, but then failed to output the file
         name, we should clean up the mess before failing.  */
      if (!dry_run && (stdout_closed = true) && close_stream (stdout) != 0)
        {
          int saved_errno = errno;
          remove (dest_name);
          error (EXIT_FAILURE, saved_errno, _("write error"));
        }
    }

  free (dest_name);
  free (template);

  exit (status);
}
