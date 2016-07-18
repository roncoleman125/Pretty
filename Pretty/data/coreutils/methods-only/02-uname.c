/* Set all the option flags according to the switches specified.
   Return the mask indicating which elements to print.  */

static int
decode_switches (int argc, char **argv)
{
  int c;
  unsigned int toprint = 0;

  if (uname_mode == UNAME_ARCH)
    {
      while ((c = getopt_long (argc, argv, "",
                               arch_long_options, NULL)) != -1)
        {
          switch (c)
            {
            case_GETOPT_HELP_CHAR;

            case_GETOPT_VERSION_CHAR (PROGRAM_NAME, ARCH_AUTHORS);

            default:
              usage (EXIT_FAILURE);
            }
        }
      toprint = PRINT_MACHINE;
    }
  else
    {
      while ((c = getopt_long (argc, argv, "asnrvmpio",
                               uname_long_options, NULL)) != -1)
        {
          switch (c)
            {
            case 'a':
              toprint = UINT_MAX;
              break;

            case 's':
              toprint |= PRINT_KERNEL_NAME;
              break;

            case 'n':
              toprint |= PRINT_NODENAME;
              break;

            case 'r':
              toprint |= PRINT_KERNEL_RELEASE;
              break;

            case 'v':
              toprint |= PRINT_KERNEL_VERSION;
              break;

            case 'm':
              toprint |= PRINT_MACHINE;
              break;

            case 'p':
              toprint |= PRINT_PROCESSOR;
              break;

            case 'i':
              toprint |= PRINT_HARDWARE_PLATFORM;
              break;

            case 'o':
              toprint |= PRINT_OPERATING_SYSTEM;
              break;

            case_GETOPT_HELP_CHAR;

            case_GETOPT_VERSION_CHAR (PROGRAM_NAME, AUTHORS);

            default:
              usage (EXIT_FAILURE);
            }
        }
    }

  if (argc != optind)
    {
      error (0, 0, _("extra operand %s"), quote (argv[optind]));
      usage (EXIT_FAILURE);
    }

  return toprint;
}

