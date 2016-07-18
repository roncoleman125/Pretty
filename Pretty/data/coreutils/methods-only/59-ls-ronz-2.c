/* Return the expected number of columns in a long-format time stamp,
   or zero if it cannot be calculated.  */

static int
long_time_expected_width (void)
{
  static int width = -1;

  if (width < 0)
    {
      time_t epoch = 0;
      struct tm const *tm = localtime (&epoch);
      char buf[TIME_STAMP_LEN_MAXIMUM + 1];

      /* In case you're wondering if localtime can fail with an input time_t
         value of 0, let's just say it's very unlikely, but not inconceivable.
         The TZ environment variable would have to specify a time zone that
         is 2**31-1900 years or more ahead of UTC.  This could happen only on
         a 64-bit system that blindly accepts e.g., TZ=UTC+20000000000000.
         However, this is not possible with Solaris 10 or glibc-2.3.5, since
         their implementations limit the offset to 167:59 and 24:00, resp.  */
      if (tm)
        {
          size_t len =
            align_nstrftime (buf, sizeof buf, long_time_format[0], tm, 0, 0);
          if (len != 0)
            width = mbsnwidth (buf, len, 0);
        }

      if (width < 0)
        width = 0;
    }

  return width;
}

