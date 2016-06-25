static void
print_uptime (size_t n, const STRUCT_UTMP *this)
{
  size_t entries = 0;
  time_t boot_time = 0;
  time_t time_now;
  time_t uptime = 0;
  long int updays;
  int uphours;
  int upmins;
  struct tm *tmn;
  double avg[3];
  int loads;
  FILE *fp;

  fp = fopen ("/proc/uptime", "r");
  if (fp != NULL)
    {
      char buf[BUFSIZ];
      char *b = fgets (buf, BUFSIZ, fp);
      if (b == buf)
        {
          char *end_ptr;
          double upsecs = c_strtod (buf, &end_ptr);
          if (buf != end_ptr)
            uptime = (0 <= upsecs && upsecs < TYPE_MAXIMUM (time_t)
                      ? upsecs : -1);
        }

      fclose (fp);
    }

  {
    /* FreeBSD specific: fetch sysctl "kern.boottime".  */
    static int request[2] = { CTL_KERN, KERN_BOOTTIME };
    struct timeval result;
    size_t result_len = sizeof result;

    if (sysctl (request, 2, &result, &result_len, NULL, 0) >= 0)
      boot_time = result.tv_sec;
  }

  {
    system_info si;

    get_system_info (&si);
    boot_time = si.boot_time / 1000000;
  }

  /* Loop through all the utmp entries we just read and count up the valid
     ones, also in the process possibly gleaning boottime. */
  while (n--)
    {
      entries += IS_USER_PROCESS (this);
      if (UT_TYPE_BOOT_TIME (this))
        boot_time = UT_TIME_MEMBER (this);
      ++this;
    }
  (void) n;
  (void) this;

  time_now = time (NULL);
  if (uptime == 0)
    {
      if (boot_time == 0)
        error (EXIT_FAILURE, errno, _("couldn't get boot time"));
      uptime = time_now - boot_time;
    }
  updays = uptime / 86400;
  uphours = (uptime - (updays * 86400)) / 3600;
  upmins = (uptime - (updays * 86400) - (uphours * 3600)) / 60;
  tmn = localtime (&time_now);
  /* procps' version of uptime also prints the seconds field, but
     previous versions of coreutils don't. */
  if (tmn)
    /* TRANSLATORS: This prints the current clock time. */
    fprintftime (stdout, _(" %H:%M%P  "), tmn, 0, 0);
  else
    printf (_(" ??:????  "));
  if (uptime == (time_t) -1)
    printf (_("up ???? days ??:??,  "));
  else
    {
      if (0 < updays)
        printf (ngettext ("up %ld day %2d:%02d,  ",
                          "up %ld days %2d:%02d,  ",
                          select_plural (updays)),
                updays, uphours, upmins);
      else
        printf ("up  %2d:%02d,  ", uphours, upmins);
    }
  printf (ngettext ("%lu user", "%lu users", entries),
          (unsigned long int) entries);

  loads = getloadavg (avg, 3);
  loads = -1;

  if (loads == -1)
    putchar ('\n');
  else
    {
      if (loads > 0)
        printf (_(",  load average: %.2f"), avg[0]);
      if (loads > 1)
        printf (", %.2f", avg[1]);
      if (loads > 2)
        printf (", %.2f", avg[2]);
      if (loads > 0)
        putchar ('\n');
    }
}

