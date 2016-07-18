/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.  */

static int
decode_switches (int argc, char **argv)
{
  char *time_style_option = NULL;

  /* Record whether there is an option specifying sort type.  */
  bool sort_type_specified = false;

  qmark_funny_chars = false;

  /* initialize all switches to default settings */

  switch (ls_mode)
    {
    case LS_MULTI_COL:
      /* This is for the `dir' program.  */
      format = many_per_line;
      set_quoting_style (NULL, escape_quoting_style);
      break;

    case LS_LONG_FORMAT:
      /* This is for the `vdir' program.  */
      format = long_format;
      set_quoting_style (NULL, escape_quoting_style);
      break;

    case LS_LS:
      /* This is for the `ls' program.  */
      if (isatty (STDOUT_FILENO))
        {
          format = many_per_line;
          /* See description of qmark_funny_chars, above.  */
          qmark_funny_chars = true;
        }
      else
        {
          format = one_per_line;
          qmark_funny_chars = false;
        }
      break;

    default:
      abort ();
    }

  time_type = time_mtime;
  sort_type = sort_name;
  sort_reverse = false;
  numeric_ids = false;
  print_block_size = false;
  indicator_style = none;
  print_inode = false;
  dereference = DEREF_UNDEFINED;
  recursive = false;
  immediate_dirs = false;
  ignore_mode = IGNORE_DEFAULT;
  ignore_patterns = NULL;
  hide_patterns = NULL;
  print_scontext = false;

  /* FIXME: put this in a function.  */
  {
    char const *q_style = getenv ("QUOTING_STYLE");
    if (q_style)
      {
        int i = ARGMATCH (q_style, quoting_style_args, quoting_style_vals);
        if (0 <= i)
          set_quoting_style (NULL, quoting_style_vals[i]);
        else
          error (0, 0,
         _("ignoring invalid value of environment variable QUOTING_STYLE: %s"),
                 quotearg (q_style));
      }
  }

  {
    char const *ls_block_size = getenv ("LS_BLOCK_SIZE");
    human_options (ls_block_size,
                   &human_output_opts, &output_block_size);
    if (ls_block_size || getenv ("BLOCK_SIZE"))
      file_output_block_size = output_block_size;
  }

  line_length = 80;
  {
    char const *p = getenv ("COLUMNS");
    if (p && *p)
      {
        unsigned long int tmp_ulong;
        if (xstrtoul (p, NULL, 0, &tmp_ulong, NULL) == LONGINT_OK
            && 0 < tmp_ulong && tmp_ulong <= SIZE_MAX)
          {
            line_length = tmp_ulong;
          }
        else
          {
            error (0, 0,
               _("ignoring invalid width in environment variable COLUMNS: %s"),
                   quotearg (p));
          }
      }
  }


  {
    struct winsize ws;

    if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws) != -1
        && 0 < ws.ws_col && ws.ws_col == (size_t) ws.ws_col)
      line_length = ws.ws_col;
  }

  {
    char const *p = getenv ("TABSIZE");
    tabsize = 8;
    if (p)
      {
        unsigned long int tmp_ulong;
        if (xstrtoul (p, NULL, 0, &tmp_ulong, NULL) == LONGINT_OK
            && tmp_ulong <= SIZE_MAX)
          {
            tabsize = tmp_ulong;
          }
        else
          {
            error (0, 0,
             _("ignoring invalid tab size in environment variable TABSIZE: %s"),
                   quotearg (p));
          }
      }
  }

  while (true)
    {
      int oi = -1;
      int c = getopt_long (argc, argv,
                           "abcdfghiklmnopqrstuvw:xABCDFGHI:LNQRST:UXZ1",
                           long_options, &oi);
      if (c == -1)
        break;

      switch (c)
        {
        case 'a':
          ignore_mode = IGNORE_MINIMAL;
          break;

        case 'b':
          set_quoting_style (NULL, escape_quoting_style);
          break;

        case 'c':
          time_type = time_ctime;
          break;

        case 'd':
          immediate_dirs = true;
          break;

        case 'f':
          /* Same as enabling -a -U and disabling -l -s.  */
          ignore_mode = IGNORE_MINIMAL;
          sort_type = sort_none;
          sort_type_specified = true;
          /* disable -l */
          if (format == long_format)
            format = (isatty (STDOUT_FILENO) ? many_per_line : one_per_line);
          print_block_size = false;	/* disable -s */
          print_with_color = false;	/* disable --color */
          break;

        case FILE_TYPE_INDICATOR_OPTION: /* --file-type */
          indicator_style = file_type;
          break;

        case 'g':
          format = long_format;
          print_owner = false;
          break;

        case 'h':
          human_output_opts = human_autoscale | human_SI | human_base_1024;
          file_output_block_size = output_block_size = 1;
          break;

        case 'i':
          print_inode = true;
          break;

        case 'k':
          human_output_opts = 0;
          file_output_block_size = output_block_size = 1024;
          break;

        case 'l':
          format = long_format;
          break;

        case 'm':
          format = with_commas;
          break;

        case 'n':
          numeric_ids = true;
          format = long_format;
          break;

        case 'o':  /* Just like -l, but don't display group info.  */
          format = long_format;
          print_group = false;
          break;

        case 'p':
          indicator_style = slash;
          break;

        case 'q':
          qmark_funny_chars = true;
          break;

        case 'r':
          sort_reverse = true;
          break;

        case 's':
          print_block_size = true;
          break;

        case 't':
          sort_type = sort_time;
          sort_type_specified = true;
          break;

        case 'u':
          time_type = time_atime;
          break;

        case 'v':
          sort_type = sort_version;
          sort_type_specified = true;
          break;

        case 'w':
          {
            unsigned long int tmp_ulong;
            if (xstrtoul (optarg, NULL, 0, &tmp_ulong, NULL) != LONGINT_OK
                || ! (0 < tmp_ulong && tmp_ulong <= SIZE_MAX))
              error (LS_FAILURE, 0, _("invalid line width: %s"),
                     quotearg (optarg));
            line_length = tmp_ulong;
            break;
          }

        case 'x':
          format = horizontal;
          break;

        case 'A':
          if (ignore_mode == IGNORE_DEFAULT)
            ignore_mode = IGNORE_DOT_AND_DOTDOT;
          break;

        case 'B':
          add_ignore_pattern ("*~");
          add_ignore_pattern (".*~");
          break;

        case 'C':
          format = many_per_line;
          break;

        case 'D':
          dired = true;
          break;

        case 'F':
          indicator_style = classify;
          break;

        case 'G':		/* inhibit display of group info */
          print_group = false;
          break;

        case 'H':
          dereference = DEREF_COMMAND_LINE_ARGUMENTS;
          break;

        case DEREFERENCE_COMMAND_LINE_SYMLINK_TO_DIR_OPTION:
          dereference = DEREF_COMMAND_LINE_SYMLINK_TO_DIR;
          break;

        case 'I':
          add_ignore_pattern (optarg);
          break;

        case 'L':
          dereference = DEREF_ALWAYS;
          break;

        case 'N':
          set_quoting_style (NULL, literal_quoting_style);
          break;

        case 'Q':
          set_quoting_style (NULL, c_quoting_style);
          break;

        case 'R':
          recursive = true;
          break;

        case 'S':
          sort_type = sort_size;
          sort_type_specified = true;
          break;

        case 'T':
          {
            unsigned long int tmp_ulong;
            if (xstrtoul (optarg, NULL, 0, &tmp_ulong, NULL) != LONGINT_OK
                || SIZE_MAX < tmp_ulong)
              error (LS_FAILURE, 0, _("invalid tab size: %s"),
                     quotearg (optarg));
            tabsize = tmp_ulong;
            break;
          }

        case 'U':
          sort_type = sort_none;
          sort_type_specified = true;
          break;

        case 'X':
          sort_type = sort_extension;
          sort_type_specified = true;
          break;

        case '1':
          /* -1 has no effect after -l.  */
          if (format != long_format)
            format = one_per_line;
          break;

        case AUTHOR_OPTION:
          print_author = true;
          break;

        case HIDE_OPTION:
          {
            struct ignore_pattern *hide = xmalloc (sizeof *hide);
            hide->pattern = optarg;
            hide->next = hide_patterns;
            hide_patterns = hide;
          }
          break;

        case SORT_OPTION:
          sort_type = XARGMATCH ("--sort", optarg, sort_args, sort_types);
          sort_type_specified = true;
          break;

        case GROUP_DIRECTORIES_FIRST_OPTION:
          directories_first = true;
          break;

        case TIME_OPTION:
          time_type = XARGMATCH ("--time", optarg, time_args, time_types);
          break;

        case FORMAT_OPTION:
          format = XARGMATCH ("--format", optarg, format_args, format_types);
          break;

        case FULL_TIME_OPTION:
          format = long_format;
          time_style_option = bad_cast ("full-iso");
          break;

        case COLOR_OPTION:
          {
            int i;
            if (optarg)
              i = XARGMATCH ("--color", optarg, color_args, color_types);
            else
              /* Using --color with no argument is equivalent to using
                 --color=always.  */
              i = color_always;

            print_with_color = (i == color_always
                                || (i == color_if_tty
                                    && isatty (STDOUT_FILENO)));

            if (print_with_color)
              {
                /* Don't use TAB characters in output.  Some terminal
                   emulators can't handle the combination of tabs and
                   color codes on the same line.  */
                tabsize = 0;
              }
            break;
          }

        case INDICATOR_STYLE_OPTION:
          indicator_style = XARGMATCH ("--indicator-style", optarg,
                                       indicator_style_args,
                                       indicator_style_types);
          break;

        case QUOTING_STYLE_OPTION:
          set_quoting_style (NULL,
                             XARGMATCH ("--quoting-style", optarg,
                                        quoting_style_args,
                                        quoting_style_vals));
          break;

        case TIME_STYLE_OPTION:
          time_style_option = optarg;
          break;

        case SHOW_CONTROL_CHARS_OPTION:
          qmark_funny_chars = false;
          break;

        case BLOCK_SIZE_OPTION:
          {
            enum strtol_error e = human_options (optarg, &human_output_opts,
                                                 &output_block_size);
            if (e != LONGINT_OK)
              xstrtol_fatal (e, oi, 0, long_options, optarg);
            file_output_block_size = output_block_size;
          }
          break;

        case SI_OPTION:
          human_output_opts = human_autoscale | human_SI;
          file_output_block_size = output_block_size = 1;
          break;

        case 'Z':
          print_scontext = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (LS_FAILURE);
        }
    }

  max_idx = MAX (1, line_length / MIN_COLUMN_WIDTH);

  filename_quoting_options = clone_quoting_options (NULL);
  if (get_quoting_style (filename_quoting_options) == escape_quoting_style)
    set_char_quoting (filename_quoting_options, ' ', 1);
  if (file_type <= indicator_style)
    {
      char const *p;
      for (p = "*=>@|" + indicator_style - file_type; *p; p++)
        set_char_quoting (filename_quoting_options, *p, 1);
    }

  dirname_quoting_options = clone_quoting_options (NULL);
  set_char_quoting (dirname_quoting_options, ':', 1);

  /* --dired is meaningful only with --format=long (-l).
     Otherwise, ignore it.  FIXME: warn about this?
     Alternatively, make --dired imply --format=long?  */
  if (dired && format != long_format)
    dired = false;

  /* If -c or -u is specified and not -l (or any other option that implies -l),
     and no sort-type was specified, then sort by the ctime (-c) or atime (-u).
     The behavior of ls when using either -c or -u but with neither -l nor -t
     appears to be unspecified by POSIX.  So, with GNU ls, `-u' alone means
     sort by atime (this is the one that's not specified by the POSIX spec),
     -lu means show atime and sort by name, -lut means show atime and sort
     by atime.  */

  if ((time_type == time_ctime || time_type == time_atime)
      && !sort_type_specified && format != long_format)
    {
      sort_type = sort_time;
    }

  if (format == long_format)
    {
      char *style = time_style_option;
      static char const posix_prefix[] = "posix-";

      if (! style)
        if (! (style = getenv ("TIME_STYLE")))
          style = bad_cast ("locale");

      while (strncmp (style, posix_prefix, sizeof posix_prefix - 1) == 0)
        {
          if (! hard_locale (LC_TIME))
            return optind;
          style += sizeof posix_prefix - 1;
        }

      if (*style == '+')
        {
          char *p0 = style + 1;
          char *p1 = strchr (p0, '\n');
          if (! p1)
            p1 = p0;
          else
            {
              if (strchr (p1 + 1, '\n'))
                error (LS_FAILURE, 0, _("invalid time style format %s"),
                       quote (p0));
              *p1++ = '\0';
            }
          long_time_format[0] = p0;
          long_time_format[1] = p1;
        }
      else
        switch (XARGMATCH ("time style", style,
                           time_style_args,
                           time_style_types))
          {
          case full_iso_time_style:
            long_time_format[0] = long_time_format[1] =
              "%Y-%m-%d %H:%M:%S.%N %z";
            break;

          case long_iso_time_style:
            long_time_format[0] = long_time_format[1] = "%Y-%m-%d %H:%M";
            break;

          case iso_time_style:
            long_time_format[0] = "%Y-%m-%d ";
            long_time_format[1] = "%m-%d %H:%M";
            break;

          case locale_time_style:
            if (hard_locale (LC_TIME))
              {
                int i;
                for (i = 0; i < 2; i++)
                  long_time_format[i] =
                    dcgettext (NULL, long_time_format[i], LC_TIME);
              }
          }
      /* Note we leave %5b etc. alone so user widths/flags are honored.  */
      if (strstr (long_time_format[0], "%b")
          || strstr (long_time_format[1], "%b"))
        if (!abmon_init ())
          error (0, 0, _("error initializing month strings"));
    }

  return optind;
}

