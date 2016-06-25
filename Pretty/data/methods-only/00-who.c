/* Return a string representing the time between WHEN and now.
   BOOTTIME is the time of last reboot.
   FIXME: locale? */
static const char *
idle_string (time_t when, time_t boottime)
{
  static time_t now = TYPE_MINIMUM (time_t);

  if (now == TYPE_MINIMUM (time_t))
    time (&now);

  if (boottime < when && now - 24 * 60 * 60 < when && when <= now)
    {
      int seconds_idle = now - when;
      if (seconds_idle < 60)
        return "  .  ";
      else
        {
          static char idle_hhmm[IDLESTR_LEN];
          sprintf (idle_hhmm, "%02d:%02d",
                   seconds_idle / (60 * 60),
                   (seconds_idle % (60 * 60)) / 60);
          return idle_hhmm;
        }
    }

  return _(" old ");
}

