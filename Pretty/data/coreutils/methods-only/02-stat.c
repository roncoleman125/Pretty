static char * ATTRIBUTE_WARN_UNUSED_RESULT
human_time (struct timespec t)
{
  static char str[MAX (INT_BUFSIZE_BOUND (intmax_t),
                       (INT_STRLEN_BOUND (int) /* YYYY */
                        + 1 /* because YYYY might equal INT_MAX + 1900 */
                        + sizeof "-MM-DD HH:MM:SS.NNNNNNNNN +ZZZZ"))];
  struct tm const *tm = localtime (&t.tv_sec);
  if (tm == NULL)
    return timetostr (t.tv_sec, str);
  nstrftime (str, sizeof str, "%Y-%m-%d %H:%M:%S.%N %z", tm, 0, t.tv_nsec);
  return str;
}

