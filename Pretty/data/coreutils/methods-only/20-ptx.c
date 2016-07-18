int
main (int argc, char **argv)
{
  int optchar;			/* argument character */
  int file_index;		/* index in text input file arrays */

  /* Decode program options.  */

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  setchrclass (NULL);

  while (optchar = getopt_long (argc, argv, "AF:GM:ORS:TW:b:i:fg:o:trw:",
                                long_options, NULL),
         optchar != EOF)
    {
      switch (optchar)
        {
        default:
          usage (EXIT_FAILURE);

        case 'G':
          gnu_extensions = false;
          break;

        case 'b':
          break_file = optarg;
          break;

        case 'f':
          ignore_case = true;
          break;

        case 'g':
          {
            unsigned long int tmp_ulong;
            if (xstrtoul (optarg, NULL, 0, &tmp_ulong, NULL) != LONGINT_OK
                || ! (0 < tmp_ulong && tmp_ulong <= INT_MAX))
              error (EXIT_FAILURE, 0, _("invalid gap width: %s"),
                     quotearg (optarg));
            gap_size = tmp_ulong;
            break;
          }

        case 'i':
          ignore_file = optarg;
          break;

        case 'o':
          only_file = optarg;
          break;

        case 'r':
          input_reference = true;
          break;

        case 't':
          /* Yet to understand...  */
          break;

        case 'w':
          {
            unsigned long int tmp_ulong;
            if (xstrtoul (optarg, NULL, 0, &tmp_ulong, NULL) != LONGINT_OK
                || ! (0 < tmp_ulong && tmp_ulong <= INT_MAX))
              error (EXIT_FAILURE, 0, _("invalid line width: %s"),
                     quotearg (optarg));
            line_width = tmp_ulong;
            break;
          }

        case 'A':
          auto_reference = true;
          break;

        case 'F':
          truncation_string = copy_unescaped_string (optarg);
          break;

        case 'M':
          macro_name = optarg;
          break;

        case 'O':
          output_format = ROFF_FORMAT;
          break;

        case 'R':
          right_reference = true;
          break;

        case 'S':
          context_regex.string = copy_unescaped_string (optarg);
          break;

        case 'T':
          output_format = TEX_FORMAT;
          break;

        case 'W':
          word_regex.string = copy_unescaped_string (optarg);
          if (!*word_regex.string)
            word_regex.string = NULL;
          break;

        case 10:
          output_format = XARGMATCH ("--format", optarg,
                                     format_args, format_vals);
        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        }
    }

  /* Process remaining arguments.  If GNU extensions are enabled, process
     all arguments as input parameters.  If disabled, accept at most two
     arguments, the second of which is an output parameter.  */

  if (optind == argc)
    {

      /* No more argument simply means: read standard input.  */

      input_file_name = xmalloc (sizeof *input_file_name);
      file_line_count = xmalloc (sizeof *file_line_count);
      number_input_files = 1;
      input_file_name[0] = NULL;
    }
  else if (gnu_extensions)
    {
      number_input_files = argc - optind;
      input_file_name = xmalloc (number_input_files * sizeof *input_file_name);
      file_line_count = xmalloc (number_input_files * sizeof *file_line_count);

      for (file_index = 0; file_index < number_input_files; file_index++)
        {
          if (!*argv[optind] || STREQ (argv[optind], "-"))
            input_file_name[file_index] = NULL;
          else
            input_file_name[file_index] = argv[optind];
          optind++;
        }
    }
  else
    {

      /* There is one necessary input file.  */

      number_input_files = 1;
      input_file_name = xmalloc (sizeof *input_file_name);
      file_line_count = xmalloc (sizeof *file_line_count);
      if (!*argv[optind] || STREQ (argv[optind], "-"))
        input_file_name[0] = NULL;
      else
        input_file_name[0] = argv[optind];
      optind++;

      /* Redirect standard output, only if requested.  */

      if (optind < argc)
        {
          if (! freopen (argv[optind], "w", stdout))
            error (EXIT_FAILURE, errno, "%s", argv[optind]);
          optind++;
        }

      /* Diagnose any other argument as an error.  */

      if (optind < argc)
        {
          error (0, 0, _("extra operand %s"), quote (argv[optind]));
          usage (EXIT_FAILURE);
        }
    }

  /* If the output format has not been explicitly selected, choose dumb
     terminal format if GNU extensions are enabled, else `roff' format.  */

  if (output_format == UNKNOWN_FORMAT)
    output_format = gnu_extensions ? DUMB_FORMAT : ROFF_FORMAT;

  /* Initialize the main tables.  */

  initialize_regex ();

  /* Read `Break character' file, if any.  */

  if (break_file)
    digest_break_file (break_file);

  /* Read `Ignore words' file and `Only words' files, if any.  If any of
     these files is empty, reset the name of the file to NULL, to avoid
     unnecessary calls to search_table. */

  if (ignore_file)
    {
      digest_word_file (ignore_file, &ignore_table);
      if (ignore_table.length == 0)
        ignore_file = NULL;
    }

  if (only_file)
    {
      digest_word_file (only_file, &only_table);
      if (only_table.length == 0)
        only_file = NULL;
    }

  /* Prepare to study all the input files.  */

  number_of_occurs[0] = 0;
  total_line_count = 0;
  maximum_word_length = 0;
  reference_max_width = 0;

  for (file_index = 0; file_index < number_input_files; file_index++)
    {

      /* Read the file in core, than study it.  */

      swallow_file_in_memory (input_file_name[file_index], &text_buffer);
      find_occurs_in_text ();

      /* Maintain for each file how many lines has been read so far when its
         end is reached.  Incrementing the count first is a simple kludge to
         handle a possible incomplete line at end of file.  */

      total_line_count++;
      file_line_count[file_index] = total_line_count;
    }

  /* Do the output process phase.  */

  sort_found_occurs ();
  fix_output_parameters ();
  generate_all_output ();

  /* All done.  */

  exit (EXIT_SUCCESS);
}
