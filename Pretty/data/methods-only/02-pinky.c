/* Return a string representing the time between WHEN and the time
   that this function is first run. */

static const char *
idle_string (time_t when)
{
  static time_t now = 0;
  static char buf[INT_STRLEN_BOUND (long int) + 2];
  time_t seconds_idle;

  if (now == 0)
    time (&now);

  seconds_idle = now - when;
  if (seconds_idle < 60)	/* One minute. */
    return "     ";
  if (seconds_idle < (24 * 60 * 60))	/* One day. */
    {
      int hours = seconds_idle / (60 * 60);
      int minutes = (seconds_idle % (60 * 60)) / 60;
      sprintf (buf, "%02d:%02d", hours, minutes);
    }
  else
    {
      unsigned long int days = seconds_idle / (24 * 60 * 60);
      sprintf (buf, "%lud", days);
    }
  return buf;
}

