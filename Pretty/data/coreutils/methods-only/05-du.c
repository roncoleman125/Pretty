/* FIXME: this code is nearly identical to code in date.c  */
/* Display the date and time in WHEN according to the format specified
   in FORMAT.  */

static void
show_date (const char *format, struct timespec when)
{
  struct tm *tm = localtime (&when.tv_sec);
  if (! tm)
    {
      char buf[INT_BUFSIZE_BOUND (intmax_t)];
      char *when_str = timetostr (when.tv_sec, buf);
      error (0, 0, _("time %s is out of range"), when_str);
      fputs (when_str, stdout);
      return;
    }

  fprintftime (stdout, format, tm, 0, when.tv_nsec);
}

