/* Estimate length of col_sep_string with option -S.  */

static void
separator_string (const char *optarg_S)
{
  col_sep_length = (int) strlen (optarg_S);
  col_sep_string = xmalloc (col_sep_length + 1);
  strcpy (col_sep_string, optarg_S);
}

int
main (int argc, char **argv)
{
  int n_files;
  bool old_options = false;
  bool old_w = false;
  bool old_s = false;
  char **file_names;

  /* Accumulate the digits of old-style options like -99.  */
  char *column_count_string = NULL;
  size_t n_digits = 0;
  size_t n_alloc = 0;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  n_files = 0;
  file_names = (argc > 1
                ? xmalloc ((argc - 1) * sizeof (char *))
                : NULL);

  while (true)
    {
      int oi = -1;
      int c = getopt_long (argc, argv, short_options, long_options, &oi);
      if (c == -1)
        break;

      if (ISDIGIT (c))
        {
          /* Accumulate column-count digits specified via old-style options. */
          if (n_digits + 1 >= n_alloc)
            column_count_string
              = X2REALLOC (column_count_string, &n_alloc);
          column_count_string[n_digits++] = c;
          column_count_string[n_digits] = '\0';
          continue;
        }

      n_digits = 0;

      switch (c)
        {
        case 1:			/* Non-option argument. */
          /* long option --page dominates old `+FIRST_PAGE ...'.  */
          if (! (first_page_number == 0
                 && *optarg == '+' && first_last_page (-2, '+', optarg + 1)))
            file_names[n_files++] = optarg;
          break;

        case PAGES_OPTION:	/* --pages=FIRST_PAGE[:LAST_PAGE] */
          {			/* dominates old opt +... */
            if (! optarg)
              error (EXIT_FAILURE, 0,
                     _("`--pages=FIRST_PAGE[:LAST_PAGE]' missing argument"));
            else if (! first_last_page (oi, 0, optarg))
              error (EXIT_FAILURE, 0, _("invalid page range %s"),
                     quote (optarg));
            break;
          }

        case COLUMNS_OPTION:	/* --columns=COLUMN */
          {
            parse_column_count (optarg);

            /* If there was a prior column count specified via the
               short-named option syntax, e.g., -9, ensure that this
               long-name-specified value overrides it.  */
            free (column_count_string);
            column_count_string = NULL;
            n_alloc = 0;
            break;
          }

        case 'a':
          print_across_flag = true;
          storing_columns = false;
          break;
        case 'b':
          balance_columns = true;
          break;
        case 'c':
          use_cntrl_prefix = true;
          break;
        case 'd':
          double_space = true;
          break;
        case 'D':
          date_format = optarg;
          break;
        case 'e':
          if (optarg)
            getoptarg (optarg, 'e', &input_tab_char,
                       &chars_per_input_tab);
          /* Could check tab width > 0. */
          untabify_input = true;
          break;
        case 'f':
        case 'F':
          use_form_feed = true;
          break;
        case 'h':
          custom_header = optarg;
          break;
        case 'i':
          if (optarg)
            getoptarg (optarg, 'i', &output_tab_char,
                       &chars_per_output_tab);
          /* Could check tab width > 0. */
          tabify_output = true;
          break;
        case 'J':
          join_lines = true;
          break;
        case 'l':
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long <= 0 || tmp_long > INT_MAX)
              {
                error (EXIT_FAILURE, 0,
                       _("`-l PAGE_LENGTH' invalid number of lines: %s"),
                       quote (optarg));
              }
            lines_per_page = tmp_long;
            break;
          }
        case 'm':
          parallel_files = true;
          storing_columns = false;
          break;
        case 'n':
          numbered_lines = true;
          if (optarg)
            getoptarg (optarg, 'n', &number_separator,
                       &chars_per_number);
          break;
        case 'N':
          skip_count = false;
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long > INT_MAX)
              {
                error (EXIT_FAILURE, 0,
                       _("`-N NUMBER' invalid starting line number: %s"),
                       quote (optarg));
              }
            start_line_num = tmp_long;
            break;
          }
        case 'o':
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long < 0 || tmp_long > INT_MAX)
              error (EXIT_FAILURE, 0,
                     _("`-o MARGIN' invalid line offset: %s"), quote (optarg));
            chars_per_margin = tmp_long;
            break;
          }
        case 'r':
          ignore_failed_opens = true;
          break;
        case 's':
          old_options = true;
          old_s = true;
          if (!use_col_separator && optarg)
            separator_string (optarg);
          break;
        case 'S':
          old_s = false;
          /* Reset an additional input of -s, -S dominates -s */
          col_sep_string = bad_cast ("");
          col_sep_length = 0;
          use_col_separator = true;
          if (optarg)
            separator_string (optarg);
          break;
        case 't':
          extremities = false;
          keep_FF = true;
          break;
        case 'T':
          extremities = false;
          keep_FF = false;
          break;
        case 'v':
          use_esc_sequence = true;
          break;
        case 'w':
          old_options = true;
          old_w = true;
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long <= 0 || tmp_long > INT_MAX)
              error (EXIT_FAILURE, 0,
                     _("`-w PAGE_WIDTH' invalid number of characters: %s"),
                     quote (optarg));
            if (!truncate_lines)
              chars_per_line = tmp_long;
            break;
          }
        case 'W':
          old_w = false;			/* dominates -w */
          truncate_lines = true;
          {
            long int tmp_long;
            if (xstrtol (optarg, NULL, 10, &tmp_long, "") != LONGINT_OK
                || tmp_long <= 0 || tmp_long > INT_MAX)
              error (EXIT_FAILURE, 0,
                     _("`-W PAGE_WIDTH' invalid number of characters: %s"),
                     quote (optarg));
            chars_per_line = tmp_long;
            break;
          }
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
          break;
        }
    }

  if (column_count_string)
    {
      parse_column_count (column_count_string);
      free (column_count_string);
    }

  if (! date_format)
    date_format = (getenv ("POSIXLY_CORRECT") && !hard_locale (LC_TIME)
                   ? "%b %e %H:%M %Y"
                   : "%Y-%m-%d %H:%M");

  /* Now we can set a reasonable initial value: */
  if (first_page_number == 0)
    first_page_number = 1;

  if (parallel_files && explicit_columns)
    error (EXIT_FAILURE, 0,
         _("cannot specify number of columns when printing in parallel"));

  if (parallel_files && print_across_flag)
    error (EXIT_FAILURE, 0,
       _("cannot specify both printing across and printing in parallel"));

/* Translate some old short options to new/long options.
   To meet downward compatibility with other UNIX pr utilities
   and some POSIX specifications. */

  if (old_options)
    {
      if (old_w)
        {
          if (parallel_files || explicit_columns)
            {
              /* activate -W */
              truncate_lines = true;
              if (old_s)
                /* adapt HP-UX and SunOS: -s = no separator;
                   activate -S */
                use_col_separator = true;
            }
          else
            /* old -w sets width with columns only
               activate -J */
            join_lines = true;
        }
      else if (!use_col_separator)
        {
          /* No -S option read */
          if (old_s && (parallel_files || explicit_columns))
            {
              if (!truncate_lines)
                {
                  /* old -s (without -w and -W) annuls column alignment,
                  uses fields, activate -J */
                  join_lines = true;
                  if (col_sep_length > 0)
                    /* activate -S */
                    use_col_separator = true;
                }
              else
                /* with -W */
                /* adapt HP-UX and SunOS: -s = no separator;
                   activate -S */
                use_col_separator = true;
            }
        }
    }

  for (; optind < argc; optind++)
    {
      file_names[n_files++] = argv[optind];
    }

  if (n_files == 0)
    {
      /* No file arguments specified;  read from standard input.  */
      print_files (0, NULL);
    }
  else
    {
      if (parallel_files)
        print_files (n_files, file_names);
      else
        {
          int i;
          for (i = 0; i < n_files; i++)
            print_files (1, &file_names[i]);
        }
    }

  cleanup ();

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, _("standard input"));
  if (failed_opens)
    exit (EXIT_FAILURE);
  exit (EXIT_SUCCESS);
}

