int
main (int argc, char **argv)
{
  int c;
  size_t len;
  bool ok = true;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  have_read_stdin = false;

  while ((c = getopt_long (argc, argv, "h:b:f:v:i:pl:s:w:n:d:", longopts,
                           NULL)) != -1)
    {
      switch (c)
        {
        case 'h':
          if (! build_type_arg (&header_type, &header_regex, header_fastmap))
            {
              error (0, 0, _("invalid header numbering style: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 'b':
          if (! build_type_arg (&body_type, &body_regex, body_fastmap))
            {
              error (0, 0, _("invalid body numbering style: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 'f':
          if (! build_type_arg (&footer_type, &footer_regex, footer_fastmap))
            {
              error (0, 0, _("invalid footer numbering style: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 'v':
          if (xstrtoimax (optarg, NULL, 10, &starting_line_number, "")
              != LONGINT_OK)
            {
              error (0, 0, _("invalid starting line number: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
  case PAGE_INCREMENT_OPTION_DEPRECATED:
    error (0, 0, _("WARNING: --page-increment is deprecated; "
                   "use --line-increment instead"));
    /* fall through */
        case 'i':
          if (! (xstrtoimax (optarg, NULL, 10, &page_incr, "") == LONGINT_OK
                 && 0 < page_incr))
            {
              error (0, 0, _("invalid line number increment: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 'p':
          reset_numbers = false;
          break;
        case 'l':
          if (! (xstrtoimax (optarg, NULL, 10, &blank_join, "") == LONGINT_OK
                 && 0 < blank_join))
            {
              error (0, 0, _("invalid number of blank lines: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 's':
          separator_str = optarg;
          break;
        case 'w':
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long <= 0 || tmp_long > INT_MAX)
              {
                error (0, 0, _("invalid line number field width: %s"),
                       quote (optarg));
                ok = false;
              }
            else
              {
                lineno_width = tmp_long;
              }
          }
          break;
        case 'n':
          if (STREQ (optarg, "ln"))
            lineno_format = FORMAT_LEFT;
          else if (STREQ (optarg, "rn"))
            lineno_format = FORMAT_RIGHT_NOLZ;
          else if (STREQ (optarg, "rz"))
            lineno_format = FORMAT_RIGHT_LZ;
          else
            {
              error (0, 0, _("invalid line numbering format: %s"),
                     quote (optarg));
              ok = false;
            }
          break;
        case 'd':
          section_del = optarg;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          ok = false;
          break;
        }
    }

  if (!ok)
    usage (EXIT_FAILURE);

  /* Initialize the section delimiters.  */
  len = strlen (section_del);

  header_del_len = len * 3;
  header_del = xmalloc (header_del_len + 1);
  stpcpy (stpcpy (stpcpy (header_del, section_del), section_del), section_del);

  body_del_len = len * 2;
  body_del = xmalloc (body_del_len + 1);
  stpcpy (stpcpy (body_del, section_del), section_del);

  footer_del_len = len;
  footer_del = xmalloc (footer_del_len + 1);
  stpcpy (footer_del, section_del);

  /* Initialize the input buffer.  */
  initbuffer (&line_buf);

  /* Initialize the printf format for unnumbered lines. */
  len = strlen (separator_str);
  print_no_line_fmt = xmalloc (lineno_width + len + 1);
  memset (print_no_line_fmt, ' ', lineno_width + len);
  print_no_line_fmt[lineno_width + len] = '\0';

  line_no = starting_line_number;
  current_type = body_type;
  current_regex = &body_regex;

  /* Main processing. */

  if (optind == argc)
    ok = nl_file ("-");
  else
    for (; optind < argc; optind++)
      ok &= nl_file (argv[optind]);

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, "-");

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
