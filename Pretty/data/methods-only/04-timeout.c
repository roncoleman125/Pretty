static unsigned long
parse_duration (const char* str)
{
  unsigned long duration;
  char *ep;

  if (xstrtoul (str, &ep, 10, &duration, NULL)
      /* Invalid interval. Note 0 disables timeout  */
      || (duration > UINT_MAX)
      /* Extra chars after the number and an optional s,m,h,d char.  */
      || (*ep && *(ep + 1))
      /* Check any suffix char and update timeout based on the suffix.  */
      || !apply_time_suffix (&duration, *ep))
    {
      error (0, 0, _("invalid time interval %s"), quote (str));
      usage (EXIT_CANCELED);
    }

  return duration;
}

