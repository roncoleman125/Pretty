/* Replace the first %b with precomputed aligned month names.
   Note on glibc-2.7 at least, this speeds up the whole `ls -lU`
   process by around 17%, compared to letting strftime() handle the %b.  */

static size_t
align_nstrftime (char *buf, size_t size, char const *fmt, struct tm const *tm,
                 int __utc, int __ns)
{
  const char *nfmt = fmt;
  /* In the unlikely event that rpl_fmt below is not large enough,
     the replacement is not done.  A malloc here slows ls down by 2%  */
  char rpl_fmt[sizeof (abmon[0]) + 100];
  const char *pb;
  if (required_mon_width && (pb = strstr (fmt, "%b")))
    {
      if (strlen (fmt) < (sizeof (rpl_fmt) - sizeof (abmon[0]) + 2))
        {
          char *pfmt = rpl_fmt;
          nfmt = rpl_fmt;

          pfmt = mempcpy (pfmt, fmt, pb - fmt);
          pfmt = stpcpy (pfmt, abmon[tm->tm_mon]);
          strcpy (pfmt, pb + 2);
        }
    }
  size_t ret = nstrftime (buf, size, nfmt, tm, __utc, __ns);
  return ret;
}

