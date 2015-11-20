/* Store into *RESULT the result of interpreting FLEX_DATE as a date,
   relative to NOW.  If NOW is null, use the current time.  */

static void
get_reldate (struct timespec *result,
             char const *flex_date, struct timespec const *now)
{
  if (! parse_datetime (result, flex_date, now))
    error (EXIT_FAILURE, 0, _("invalid date format %s"), quote (flex_date));
}

