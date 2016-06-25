int
main (int argc, char **argv)
{
  unsigned char bin_buffer_unaligned[DIGEST_BIN_BYTES + DIGEST_ALIGN];
  /* Make sure bin_buffer is properly aligned. */
  unsigned char *bin_buffer = ptr_align (bin_buffer_unaligned, DIGEST_ALIGN);
  bool do_check = false;
  int opt;
  bool ok = true;
  int binary = -1;

  /* Setting values of global variables.  */
  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  /* Line buffer stdout to ensure lines are written atomically and immediately
     so that processes running in parallel do not intersperse their output.  */
  setvbuf (stdout, NULL, _IOLBF, 0);

  while ((opt = getopt_long (argc, argv, "bctw", long_options, NULL)) != -1)
    switch (opt)
      {
      case 'b':
        binary = 1;
        break;
      case 'c':
        do_check = true;
        break;
      case STATUS_OPTION:
        status_only = true;
        warn = false;
        quiet = false;
        break;
      case 't':
        binary = 0;
        break;
      case 'w':
        status_only = false;
        warn = true;
        quiet = false;
        break;
      case QUIET_OPTION:
        status_only = false;
        warn = false;
        quiet = true;
        break;
      case_GETOPT_HELP_CHAR;
      case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
      default:
        usage (EXIT_FAILURE);
      }

  min_digest_line_length = MIN_DIGEST_LINE_LENGTH;
  digest_hex_bytes = DIGEST_HEX_BYTES;

  if (0 <= binary && do_check)
    {
      error (0, 0, _("the --binary and --text options are meaningless when "
                     "verifying checksums"));
      usage (EXIT_FAILURE);
    }

  if (status_only && !do_check)
    {
      error (0, 0,
       _("the --status option is meaningful only when verifying checksums"));
      usage (EXIT_FAILURE);
    }

  if (warn && !do_check)
    {
      error (0, 0,
       _("the --warn option is meaningful only when verifying checksums"));
      usage (EXIT_FAILURE);
    }

  if (quiet && !do_check)
    {
      error (0, 0,
       _("the --quiet option is meaningful only when verifying checksums"));
      usage (EXIT_FAILURE);
    }

  if (!O_BINARY && binary < 0)
    binary = 0;

  if (optind == argc)
    argv[argc++] = bad_cast ("-");

  for (; optind < argc; ++optind)
    {
      char *file = argv[optind];

      if (do_check)
        ok &= digest_check (file);
      else
        {
          int file_is_binary = binary;

          if (! digest_file (file, &file_is_binary, bin_buffer))
            ok = false;
          else
            {
              size_t i;

              /* Output a leading backslash if the file name contains
                 a newline or backslash.  */
              if (strchr (file, '\n') || strchr (file, '\\'))
                putchar ('\\');

              for (i = 0; i < (digest_hex_bytes / 2); ++i)
                printf ("%02x", bin_buffer[i]);

              putchar (' ');
              if (file_is_binary)
                putchar ('*');
              else
                putchar (' ');

              /* Translate each NEWLINE byte to the string, "\\n",
                 and each backslash to "\\\\".  */
              for (i = 0; i < strlen (file); ++i)
                {
                  switch (file[i])
                    {
                    case '\n':
                      fputs ("\\n", stdout);
                      break;

                    case '\\':
                      fputs ("\\\\", stdout);
                      break;

                    default:
                      putchar (file[i]);
                      break;
                    }
                }
              putchar ('\n');
            }
        }
    }

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, _("standard input"));

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
