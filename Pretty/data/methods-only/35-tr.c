int
main (int argc, char **argv)
{
  int c;
  int non_option_args;
  int min_operands;
  int max_operands;
  struct Spec_list buf1, buf2;
  struct Spec_list *s1 = &buf1;
  struct Spec_list *s2 = &buf2;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((c = getopt_long (argc, argv, "+cCdst", long_options, NULL)) != -1)
    {
      switch (c)
        {
        case 'c':
        case 'C':
          complement = true;
          break;

        case 'd':
          delete = true;
          break;

        case 's':
          squeeze_repeats = true;
          break;

        case 't':
          truncate_set1 = true;
          break;

        case_GETOPT_HELP_CHAR;

        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

        default:
          usage (EXIT_FAILURE);
          break;
        }
    }

  non_option_args = argc - optind;
  translating = (non_option_args == 2 && !delete);
  min_operands = 1 + (delete == squeeze_repeats);
  max_operands = 1 + (delete <= squeeze_repeats);

  if (non_option_args < min_operands)
    {
      if (non_option_args == 0)
        error (0, 0, _("missing operand"));
      else
        {
          error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
          fprintf (stderr, "%s\n",
                   _(squeeze_repeats
                     ? N_("Two strings must be given when "
                          "both deleting and squeezing repeats.")
                     : N_("Two strings must be given when translating.")));
        }
      usage (EXIT_FAILURE);
    }

  if (max_operands < non_option_args)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind + max_operands]));
      if (non_option_args == 2)
        fprintf (stderr, "%s\n",
                 _("Only one string may be given when "
                   "deleting without squeezing repeats."));
      usage (EXIT_FAILURE);
    }

  spec_init (s1);
  if (!parse_str (argv[optind], s1))
    exit (EXIT_FAILURE);

  if (non_option_args == 2)
    {
      spec_init (s2);
      if (!parse_str (argv[optind + 1], s2))
        exit (EXIT_FAILURE);
    }
  else
    s2 = NULL;

  validate (s1, s2);

  /* Use binary I/O, since `tr' is sometimes used to transliterate
     non-printable characters, or characters which are stripped away
     by text-mode reads (like CR and ^Z).  */
  if (O_BINARY && ! isatty (STDIN_FILENO))
    xfreopen (NULL, "rb", stdin);
  if (O_BINARY && ! isatty (STDOUT_FILENO))
    xfreopen (NULL, "wb", stdout);

  fadvise (stdin, FADVISE_SEQUENTIAL);

  if (squeeze_repeats && non_option_args == 1)
    {
      set_initialize (s1, complement, in_squeeze_set);
      squeeze_filter (io_buf, sizeof io_buf, plain_read);
    }
  else if (delete && non_option_args == 1)
    {
      set_initialize (s1, complement, in_delete_set);

      while (true)
        {
          size_t nr = read_and_delete (io_buf, sizeof io_buf);
          if (nr == 0)
            break;
          if (fwrite (io_buf, 1, nr, stdout) != nr)
            error (EXIT_FAILURE, errno, _("write error"));
        }
    }
  else if (squeeze_repeats && delete && non_option_args == 2)
    {
      set_initialize (s1, complement, in_delete_set);
      set_initialize (s2, false, in_squeeze_set);
      squeeze_filter (io_buf, sizeof io_buf, read_and_delete);
    }
  else if (translating)
    {
      if (complement)
        {
          int i;
          bool *in_s1 = in_delete_set;

          set_initialize (s1, false, in_s1);
          s2->state = BEGIN_STATE;
          for (i = 0; i < N_CHARS; i++)
            xlate[i] = i;
          for (i = 0; i < N_CHARS; i++)
            {
              if (!in_s1[i])
                {
                  int ch = get_next (s2, NULL);
                  assert (ch != -1 || truncate_set1);
                  if (ch == -1)
                    {
                      /* This will happen when tr is invoked like e.g.
                         tr -cs A-Za-z0-9 '\012'.  */
                      break;
                    }
                  xlate[i] = ch;
                }
            }
        }
      else
        {
          int c1, c2;
          int i;
          enum Upper_Lower_class class_s1;
          enum Upper_Lower_class class_s2;

          for (i = 0; i < N_CHARS; i++)
            xlate[i] = i;
          s1->state = BEGIN_STATE;
          s2->state = BEGIN_STATE;
          while (true)
            {
              c1 = get_next (s1, &class_s1);
              c2 = get_next (s2, &class_s2);

              if (class_s1 == UL_LOWER && class_s2 == UL_UPPER)
                {
                  for (i = 0; i < N_CHARS; i++)
                    if (islower (i))
                      xlate[i] = toupper (i);
                }
              else if (class_s1 == UL_UPPER && class_s2 == UL_LOWER)
                {
                  for (i = 0; i < N_CHARS; i++)
                    if (isupper (i))
                      xlate[i] = tolower (i);
                }
              else
                {
                  /* The following should have been checked by validate...  */
                  if (c1 == -1 || c2 == -1)
                    break;
                  xlate[c1] = c2;
                }

              /* When case-converting, skip the elements as an optimization.  */
              if (class_s2 != UL_NONE)
                {
                  skip_construct (s1);
                  skip_construct (s2);
                }
            }
          assert (c1 == -1 || truncate_set1);
        }
      if (squeeze_repeats)
        {
          set_initialize (s2, false, in_squeeze_set);
          squeeze_filter (io_buf, sizeof io_buf, read_and_xlate);
        }
      else
        {
          while (true)
            {
              size_t bytes_read = read_and_xlate (io_buf, sizeof io_buf);
              if (bytes_read == 0)
                break;
              if (fwrite (io_buf, 1, bytes_read, stdout) != bytes_read)
                error (EXIT_FAILURE, errno, _("write error"));
            }
        }
    }

  if (close (STDIN_FILENO) != 0)
    error (EXIT_FAILURE, errno, _("standard input"));

  exit (EXIT_SUCCESS);
}
