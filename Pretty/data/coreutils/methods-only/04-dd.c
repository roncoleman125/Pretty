/* Print transfer statistics.  */

static void
print_stats (void)
{
  xtime_t now = gethrxtime ();
  char hbuf[LONGEST_HUMAN_READABLE + 1];
  int human_opts =
    (human_autoscale | human_round_to_nearest
     | human_space_before_unit | human_SI | human_B);
  double delta_s;
  char const *bytes_per_second;

  fprintf (stderr,
           _("%"PRIuMAX"+%"PRIuMAX" records in\n"
             "%"PRIuMAX"+%"PRIuMAX" records out\n"),
           r_full, r_partial, w_full, w_partial);

  if (r_truncate != 0)
    fprintf (stderr,
             ngettext ("%"PRIuMAX" truncated record\n",
                       "%"PRIuMAX" truncated records\n",
                       select_plural (r_truncate)),
             r_truncate);

  if (status_flags & STATUS_NOXFER)
    return;

  /* Use integer arithmetic to compute the transfer rate,
     since that makes it easy to use SI abbreviations.  */

  fprintf (stderr,
           ngettext ("%"PRIuMAX" byte (%s) copied",
                     "%"PRIuMAX" bytes (%s) copied",
                     select_plural (w_bytes)),
           w_bytes,
           human_readable (w_bytes, hbuf, human_opts, 1, 1));

  if (start_time < now)
    {
      double XTIME_PRECISIONe0 = XTIME_PRECISION;
      uintmax_t delta_xtime = now;
      delta_xtime -= start_time;
      delta_s = delta_xtime / XTIME_PRECISIONe0;
      bytes_per_second = human_readable (w_bytes, hbuf, human_opts,
                                         XTIME_PRECISION, delta_xtime);
    }
  else
    {
      delta_s = 0;
      bytes_per_second = _("Infinity B");
    }

  /* TRANSLATORS: The two instances of "s" in this string are the SI
     symbol "s" (meaning second), and should not be translated.

     This format used to be:

     ngettext (", %g second, %s/s\n", ", %g seconds, %s/s\n", delta_s == 1)

     but that was incorrect for languages like Polish.  To fix this
     bug we now use SI symbols even though they're a bit more
     confusing in English.  */
  fprintf (stderr, _(", %g s, %s/s\n"), delta_s, bytes_per_second);
}

