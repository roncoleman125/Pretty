int
main (int argc, char **argv)
{
  int n_files;
  size_t i;
  int l_c_m;
  size_t desired_width IF_LINT ( = 0);
  bool modern = false;
  bool width_specified = false;
  bool ok = true;
  size_t width_per_block = 0;
  static char const multipliers[] = "bEGKkMmPTYZ0";

  /* The old-style `pseudo starting address' to be printed in parentheses
     after any true address.  */
  uintmax_t pseudo_start IF_LINT ( = 0);

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  for (i = 0; i <= MAX_INTEGRAL_TYPE_SIZE; i++)
    integral_type_size[i] = NO_SIZE;

  integral_type_size[sizeof (char)] = CHAR;
  integral_type_size[sizeof (short int)] = SHORT;
  integral_type_size[sizeof (int)] = INT;
  integral_type_size[sizeof (long int)] = LONG;
  /* If `long int' and `long long int' have the same size, it's fine
     to overwrite the entry for `long' with this one.  */
  integral_type_size[sizeof (unsigned_long_long_int)] = LONG_LONG;

  for (i = 0; i <= MAX_FP_TYPE_SIZE; i++)
    fp_type_size[i] = NO_SIZE;

  fp_type_size[sizeof (float)] = FLOAT_SINGLE;
  /* The array entry for `double' is filled in after that for `long double'
     so that if they are the same size, we avoid any overhead of
     long double computation in libc.  */
  fp_type_size[sizeof (long double)] = FLOAT_LONG_DOUBLE;
  fp_type_size[sizeof (double)] = FLOAT_DOUBLE;

  n_specs = 0;
  n_specs_allocated = 0;
  spec = NULL;

  format_address = format_address_std;
  address_base = 8;
  address_pad_len = 7;
  flag_dump_strings = false;

  while (true)
    {
      uintmax_t tmp;
      enum strtol_error s_err;
      int oi = -1;
      int c = getopt_long (argc, argv, short_options, long_options, &oi);
      if (c == -1)
        break;

      switch (c)
        {
        case 'A':
          modern = true;
          switch (optarg[0])
            {
            case 'd':
              format_address = format_address_std;
              address_base = 10;
              address_pad_len = 7;
              break;
            case 'o':
              format_address = format_address_std;
              address_base = 8;
              address_pad_len = 7;
              break;
            case 'x':
              format_address = format_address_std;
              address_base = 16;
              address_pad_len = 6;
              break;
            case 'n':
              format_address = format_address_none;
              address_pad_len = 0;
              break;
            default:
              error (EXIT_FAILURE, 0,
                     _("invalid output address radix `%c'; \
it must be one character from [doxn]"),
                     optarg[0]);
              break;
            }
          break;

        case 'j':
          modern = true;
          s_err = xstrtoumax (optarg, NULL, 0, &n_bytes_to_skip, multipliers);
          if (s_err != LONGINT_OK)
            xstrtol_fatal (s_err, oi, c, long_options, optarg);
          break;

        case 'N':
          modern = true;
          limit_bytes_to_format = true;

          s_err = xstrtoumax (optarg, NULL, 0, &max_bytes_to_format,
                              multipliers);
          if (s_err != LONGINT_OK)
            xstrtol_fatal (s_err, oi, c, long_options, optarg);
          break;

        case 'S':
          modern = true;
          if (optarg == NULL)
            string_min = 3;
          else
            {
              s_err = xstrtoumax (optarg, NULL, 0, &tmp, multipliers);
              if (s_err != LONGINT_OK)
                xstrtol_fatal (s_err, oi, c, long_options, optarg);

              /* The minimum string length may be no larger than SIZE_MAX,
                 since we may allocate a buffer of this size.  */
              if (SIZE_MAX < tmp)
                error (EXIT_FAILURE, 0, _("%s is too large"), optarg);

              string_min = tmp;
            }
          flag_dump_strings = true;
          break;

        case 't':
          modern = true;
          ok &= decode_format_string (optarg);
          break;

        case 'v':
          modern = true;
          abbreviate_duplicate_blocks = false;
          break;

        case TRADITIONAL_OPTION:
          traditional = true;
          break;

          /* The next several cases map the traditional format
             specification options to the corresponding modern format
             specs.  GNU od accepts any combination of old- and
             new-style options.  Format specification options accumulate.
             The obsolescent and undocumented formats are compatible
             with FreeBSD 4.10 od.  */

        case old_char:					\
          ok &= decode_format_string (new_string);	\
          break

          CASE_OLD_ARG ('a', "a");
          CASE_OLD_ARG ('b', "o1");
          CASE_OLD_ARG ('c', "c");
          CASE_OLD_ARG ('D', "u4"); /* obsolescent and undocumented */
          CASE_OLD_ARG ('d', "u2");
        case 'F': /* obsolescent and undocumented alias */
          CASE_OLD_ARG ('e', "fD"); /* obsolescent and undocumented */
          CASE_OLD_ARG ('f', "fF");
        case 'X': /* obsolescent and undocumented alias */
          CASE_OLD_ARG ('H', "x4"); /* obsolescent and undocumented */
          CASE_OLD_ARG ('i', "dI");
        case 'I': case 'L': /* obsolescent and undocumented aliases */
          CASE_OLD_ARG ('l', "dL");
          CASE_OLD_ARG ('O', "o4"); /* obsolesent and undocumented */
        case 'B': /* obsolescent and undocumented alias */
          CASE_OLD_ARG ('o', "o2");
          CASE_OLD_ARG ('s', "d2");
        case 'h': /* obsolescent and undocumented alias */
          CASE_OLD_ARG ('x', "x2");


        case 'w':
          modern = true;
          width_specified = true;
          if (optarg == NULL)
            {
              desired_width = 32;
            }
          else
            {
              uintmax_t w_tmp;
              s_err = xstrtoumax (optarg, NULL, 10, &w_tmp, "");
              if (s_err != LONGINT_OK)
                xstrtol_fatal (s_err, oi, c, long_options, optarg);
              if (SIZE_MAX < w_tmp)
                error (EXIT_FAILURE, 0, _("%s is too large"), optarg);
              desired_width = w_tmp;
            }
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
          break;
        }
    }

  if (!ok)
    exit (EXIT_FAILURE);

  if (flag_dump_strings && n_specs > 0)
    error (EXIT_FAILURE, 0,
           _("no type may be specified when dumping strings"));

  n_files = argc - optind;

  /* If the --traditional option is used, there may be from
     0 to 3 remaining command line arguments;  handle each case
     separately.
        od [file] [[+]offset[.][b] [[+]label[.][b]]]
     The offset and label have the same syntax.

     If --traditional is not given, and if no modern options are
     given, and if the offset begins with + or (if there are two
     operands) a digit, accept only this form, as per POSIX:
        od [file] [[+]offset[.][b]]
  */

  if (!modern || traditional)
    {
      uintmax_t o1;
      uintmax_t o2;

      switch (n_files)
        {
        case 1:
          if ((traditional || argv[optind][0] == '+')
              && parse_old_offset (argv[optind], &o1))
            {
              n_bytes_to_skip = o1;
              --n_files;
              ++argv;
            }
          break;

        case 2:
          if ((traditional || argv[optind + 1][0] == '+'
               || ISDIGIT (argv[optind + 1][0]))
              && parse_old_offset (argv[optind + 1], &o2))
            {
              if (traditional && parse_old_offset (argv[optind], &o1))
                {
                  n_bytes_to_skip = o1;
                  flag_pseudo_start = true;
                  pseudo_start = o2;
                  argv += 2;
                  n_files -= 2;
                }
              else
                {
                  n_bytes_to_skip = o2;
                  --n_files;
                  argv[optind + 1] = argv[optind];
                  ++argv;
                }
            }
          break;

        case 3:
          if (traditional
              && parse_old_offset (argv[optind + 1], &o1)
              && parse_old_offset (argv[optind + 2], &o2))
            {
              n_bytes_to_skip = o1;
              flag_pseudo_start = true;
              pseudo_start = o2;
              argv[optind + 2] = argv[optind];
              argv += 2;
              n_files -= 2;
            }
          break;
        }

      if (traditional && 1 < n_files)
        {
          error (0, 0, _("extra operand %s"), quote (argv[optind + 1]));
          error (0, 0, "%s",
                 _("compatibility mode supports at most one file"));
          usage (EXIT_FAILURE);
        }
    }

  if (flag_pseudo_start)
    {
      if (format_address == format_address_none)
        {
          address_base = 8;
          address_pad_len = 7;
          format_address = format_address_paren;
        }
      else
        format_address = format_address_label;
    }

  if (limit_bytes_to_format)
    {
      end_offset = n_bytes_to_skip + max_bytes_to_format;
      if (end_offset < n_bytes_to_skip)
        error (EXIT_FAILURE, 0, _("skip-bytes + read-bytes is too large"));
    }

  if (n_specs == 0)
    decode_format_string ("oS");

  if (n_files > 0)
    {
      /* Set the global pointer FILE_LIST so that it
         references the first file-argument on the command-line.  */

      file_list = (char const *const *) &argv[optind];
    }
  else
    {
      /* No files were listed on the command line.
         Set the global pointer FILE_LIST so that it
         references the null-terminated list of one name: "-".  */

      file_list = default_file_list;
    }

  /* open the first input file */
  ok = open_next_file ();
  if (in_stream == NULL)
    goto cleanup;

  /* skip over any unwanted header bytes */
  ok &= skip (n_bytes_to_skip);
  if (in_stream == NULL)
    goto cleanup;

  pseudo_offset = (flag_pseudo_start ? pseudo_start - n_bytes_to_skip : 0);

  /* Compute output block length.  */
  l_c_m = get_lcm ();

  if (width_specified)
    {
      if (desired_width != 0 && desired_width % l_c_m == 0)
        bytes_per_block = desired_width;
      else
        {
          error (0, 0, _("warning: invalid width %lu; using %d instead"),
                 (unsigned long int) desired_width, l_c_m);
          bytes_per_block = l_c_m;
        }
    }
  else
    {
      if (l_c_m < DEFAULT_BYTES_PER_BLOCK)
        bytes_per_block = l_c_m * (DEFAULT_BYTES_PER_BLOCK / l_c_m);
      else
        bytes_per_block = l_c_m;
    }

  /* Compute padding necessary to align output block.  */
  for (i = 0; i < n_specs; i++)
    {
      int fields_per_block = bytes_per_block / width_bytes[spec[i].size];
      int block_width = (spec[i].field_width + 1) * fields_per_block;
      if (width_per_block < block_width)
        width_per_block = block_width;
    }
  for (i = 0; i < n_specs; i++)
    {
      int fields_per_block = bytes_per_block / width_bytes[spec[i].size];
      int block_width = spec[i].field_width * fields_per_block;
      spec[i].pad_width = width_per_block - block_width;
    }

  printf ("lcm=%d, width_per_block=%zu\n", l_c_m, width_per_block);
  for (i = 0; i < n_specs; i++)
    {
      int fields_per_block = bytes_per_block / width_bytes[spec[i].size];
      assert (bytes_per_block % width_bytes[spec[i].size] == 0);
      assert (1 <= spec[i].pad_width / fields_per_block);
      printf ("%d: fmt=\"%s\" in_width=%d out_width=%d pad=%d\n",
              i, spec[i].fmt_string, width_bytes[spec[i].size],
              spec[i].field_width, spec[i].pad_width);
    }

  ok &= (flag_dump_strings ? dump_strings () : dump ());

cleanup:;

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, _("standard input"));

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
