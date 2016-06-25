int
main (int argc, char **argv)
{
  mode_t newmode;
  char const *specified_mode = NULL;
  int optc;
  int expected_operands;
  mode_t node_type;
  security_context_t scontext = NULL;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  while ((optc = getopt_long (argc, argv, "m:Z:", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'm':
          specified_mode = optarg;
          break;
        case 'Z':
          scontext = optarg;
          break;
        case_GETOPT_HELP_CHAR;
        case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);
        default:
          usage (EXIT_FAILURE);
        }
    }

  newmode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (specified_mode)
    {
      struct mode_change *change = mode_compile (specified_mode);
      if (!change)
        error (EXIT_FAILURE, 0, _("invalid mode"));
      newmode = mode_adjust (newmode, false, umask (0), change, NULL);
      free (change);
      if (newmode & ~S_IRWXUGO)
        error (EXIT_FAILURE, 0,
               _("mode must specify only file permission bits"));
    }

  /* If the number of arguments is 0 or 1,
     or (if it's 2 or more and the second one starts with `p'), then there
     must be exactly two operands.  Otherwise, there must be four.  */
  expected_operands = (argc <= optind
                       || (optind + 1 < argc && argv[optind + 1][0] == 'p')
                       ? 2 : 4);

  if (argc - optind < expected_operands)
    {
      if (argc <= optind)
        error (0, 0, _("missing operand"));
      else
        error (0, 0, _("missing operand after %s"), quote (argv[argc - 1]));
      if (expected_operands == 4 && argc - optind == 2)
        fprintf (stderr, "%s\n",
                 _("Special files require major and minor device numbers."));
      usage (EXIT_FAILURE);
    }

  if (expected_operands < argc - optind)
    {
      error (0, 0, _("extra operand %s"),
             quote (argv[optind + expected_operands]));
      if (expected_operands == 2 && argc - optind == 4)
        fprintf (stderr, "%s\n",
                 _("Fifos do not have major and minor device numbers."));
      usage (EXIT_FAILURE);
    }

  if (scontext && setfscreatecon (scontext) < 0)
    error (EXIT_FAILURE, errno,
           _("failed to set default file creation context to %s"),
           quote (scontext));

  /* Only check the first character, to allow mnemonic usage like
     `mknod /dev/rst0 character 18 0'. */

  switch (argv[optind + 1][0])
    {
    case 'b':			/* `block' or `buffered' */
      error (EXIT_FAILURE, 0, _("block special files not supported"));
      node_type = S_IFBLK;
      goto block_or_character;

    case 'c':			/* `character' */
    case 'u':			/* `unbuffered' */
      error (EXIT_FAILURE, 0, _("character special files not supported"));
      node_type = S_IFCHR;
      goto block_or_character;

    block_or_character:
      {
        char const *s_major = argv[optind + 2];
        char const *s_minor = argv[optind + 3];
        uintmax_t i_major, i_minor;
        dev_t device;

        if (xstrtoumax (s_major, NULL, 0, &i_major, NULL) != LONGINT_OK
            || i_major != (major_t) i_major)
          error (EXIT_FAILURE, 0,
                 _("invalid major device number %s"), quote (s_major));

        if (xstrtoumax (s_minor, NULL, 0, &i_minor, NULL) != LONGINT_OK
            || i_minor != (minor_t) i_minor)
          error (EXIT_FAILURE, 0,
                 _("invalid minor device number %s"), quote (s_minor));

        device = makedev (i_major, i_minor);
        if (device == NODEV)
          error (EXIT_FAILURE, 0, _("invalid device %s %s"), s_major, s_minor);

        if (mknod (argv[optind], newmode | node_type, device) != 0)
          error (EXIT_FAILURE, errno, "%s", quote (argv[optind]));
      }
      break;

    case 'p':			/* `pipe' */
      if (mkfifo (argv[optind], newmode) != 0)
        error (EXIT_FAILURE, errno, "%s", quote (argv[optind]));
      break;

    default:
      error (0, 0, _("invalid device type %s"), quote (argv[optind + 1]));
      usage (EXIT_FAILURE);
    }

  exit (EXIT_SUCCESS);
}
