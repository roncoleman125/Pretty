int
main (int argc, char **argv)
{
  static char const unknown[] = "unknown";

  /* Mask indicating which elements to print. */
  unsigned int toprint = 0;

  initialize_main (&argc, &argv);
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  toprint = decode_switches (argc, argv);

  if (toprint == 0)
    toprint = PRINT_KERNEL_NAME;

  if (toprint
       & (PRINT_KERNEL_NAME | PRINT_NODENAME | PRINT_KERNEL_RELEASE
          | PRINT_KERNEL_VERSION | PRINT_MACHINE))
    {
      struct utsname name;

      if (uname (&name) == -1)
        error (EXIT_FAILURE, errno, _("cannot get system name"));

      if (toprint & PRINT_KERNEL_NAME)
        print_element (name.sysname);
      if (toprint & PRINT_NODENAME)
        print_element (name.nodename);
      if (toprint & PRINT_KERNEL_RELEASE)
        print_element (name.release);
      if (toprint & PRINT_KERNEL_VERSION)
        print_element (name.version);
      if (toprint & PRINT_MACHINE)
        print_element (name.machine);
    }

  if (toprint & PRINT_PROCESSOR)
    {
      char const *element = unknown;
      {
        static char processor[257];
        if (0 <= sysinfo (SI_ARCHITECTURE, processor, sizeof processor))
          element = processor;
      }
      if (element == unknown)
        {
          static char processor[257];
          size_t s = sizeof processor;
          static int mib[] = { CTL_HW, UNAME_PROCESSOR };
          if (sysctl (mib, 2, processor, &s, 0, 0) >= 0)
            element = processor;

          /* This kludge works around a bug in Mac OS X.  */
          if (element == unknown)
            {
              cpu_type_t cputype;
              size_t s = sizeof cputype;
              NXArchInfo const *ai;
              if (sysctlbyname ("hw.cputype", &cputype, &s, NULL, 0) == 0
                  && (ai = NXGetArchInfoFromCpuType (cputype,
                                                     CPU_SUBTYPE_MULTIPLE))
                  != NULL)
                element = ai->name;

              /* Hack "safely" around the ppc vs. powerpc return value. */
              if (cputype == CPU_TYPE_POWERPC
                  && strncmp (element, "ppc", 3) == 0)
                element = "powerpc";
            }
        }
      if (! (toprint == UINT_MAX && element == unknown))
        print_element (element);
    }

  if (toprint & PRINT_HARDWARE_PLATFORM)
    {
      char const *element = unknown;
      {
        static char hardware_platform[257];
        if (0 <= sysinfo (SI_PLATFORM,
                          hardware_platform, sizeof hardware_platform))
          element = hardware_platform;
      }
      if (element == unknown)
        {
          static char hardware_platform[257];
          size_t s = sizeof hardware_platform;
          static int mib[] = { CTL_HW, UNAME_HARDWARE_PLATFORM };
          if (sysctl (mib, 2, hardware_platform, &s, 0, 0) >= 0)
            element = hardware_platform;
        }
      if (! (toprint == UINT_MAX && element == unknown))
        print_element (element);
    }

  if (toprint & PRINT_OPERATING_SYSTEM)
    print_element (HOST_OPERATING_SYSTEM);

  putchar ('\n');

  exit (EXIT_SUCCESS);
}
