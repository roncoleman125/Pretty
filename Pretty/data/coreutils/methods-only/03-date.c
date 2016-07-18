/* Display the date and/or time in WHEN according to the format specified
   in FORMAT, followed by a newline.  Return true if successful.  */

static bool
show_date (const char *format, struct timespec when)
{
  struct tm *tm;

  tm = localtime (&when.tv_sec);
  if (! tm)
    {
      char buf[INT_BUFSIZE_BOUND (intmax_t)];
      error (0, 0, _("time %s is out of range"), timetostr (when.tv_sec, buf));
      return false;
    }

  if (format == rfc_2822_format)
    setlocale (LC_TIME, "C");
  fprintftime (stdout, format, tm, 0, when.tv_nsec);
  fputc ('\n', stdout);
  if (format == rfc_2822_format)
    setlocale (LC_TIME, "");

  return true;
}
