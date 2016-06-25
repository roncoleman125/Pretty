int
main (int argc, char **argv)
{
  int optc;
  bool ok;
  bool delim_specified = false;
  char *spec_list_string IF_LINT ( = NULL);

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  operating_mode = undefined_mode;

  /* By default, all non-delimited lines are printed.  */
  suppress_non_delimited = false;

  delim = '\0';
  have_read_stdin = false;

  while ((optc = getopt_long (argc, argv, "b:c:d:f:ns", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'b':
        case 'c':
          /* Build the byte list. */
          if (operating_mode != undefined_mode)
            FATAL_ERROR (_("only one type of list may be specified"));
          operating_mode = byte_mode;
          spec_list_string = optarg;
          break;

        case 'f':
          /* Build the field list. */
          if (operating_mode != undefined_mode)
            FATAL_ERROR (_("only one type of list may be specified"));
          operating_mode = field_mode;
          spec_list_string = optarg;
          break;

        case 'd':
          /* New delimiter. */
          /* Interpret -d '' to mean `use the NUL byte as the delimiter.'  */
          if (optarg[0] != '\0' && optarg[1] != '\0')
            FATAL_ERROR (_("the delimiter must be a single character"));
          delim = optarg[0];
          delim_specified = true;
          break;

        case OUTPUT_DELIMITER_OPTION:
          output_delimiter_specified = true;
          /* Interpret --output-delimiter='' to mean
             `use the NUL byte as the delimiter.'  */
          output_delimiter_length = (optarg[0] == '\0'
                                     ? 1 : strlen (optarg));
          output_delimiter_string = xstrdup (optarg);
          break;

        case 'n':
          break;

        case 's':
          suppress_non_delimited = true;
          break;

        case COMPLEMENT_OPTION:
          complement = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
        }
    }

  if (operating_mode == undefined_mode)
    FATAL_ERROR (_("you must specify a list of bytes, characters, or fields"));

  if (delim != '\0' && operating_mode != field_mode)
    FATAL_ERROR (_("an input delimiter may be specified only\
 when operating on fields"));

  if (suppress_non_delimited && operating_mode != field_mode)
    FATAL_ERROR (_("suppressing non-delimited lines makes sense\n\
\tonly when operating on fields"));

  if (output_delimiter_specified)
    {
      range_start_ht = hash_initialize (HT_RANGE_START_INDEX_INITIAL_CAPACITY,
                                        NULL, hash_int,
                                        hash_compare_ints, NULL);
      if (range_start_ht == NULL)
        xalloc_die ();

    }

  if (! set_fields (spec_list_string))
    {
      if (operating_mode == field_mode)
        FATAL_ERROR (_("missing list of fields"));
      else
        FATAL_ERROR (_("missing list of positions"));
    }

  if (!delim_specified)
    delim = '\t';

  if (output_delimiter_string == NULL)
    {
      static char dummy[2];
      dummy[0] = delim;
      dummy[1] = '\0';
      output_delimiter_string = dummy;
      output_delimiter_length = 1;
    }

  if (optind == argc)
    ok = cut_file ("-");
  else
    for (ok = true; optind < argc; optind++)
      ok &= cut_file (argv[optind]);

  if (range_start_ht)
    hash_free (range_start_ht);

  if (have_read_stdin && fclose (stdin) == EOF)
    {
      error (0, errno, "-");
      ok = false;
    }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
