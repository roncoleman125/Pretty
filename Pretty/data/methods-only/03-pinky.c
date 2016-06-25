/* Return a time string.  */
static const char *
time_string (const STRUCT_UTMP *utmp_ent)
{
  static char buf[INT_STRLEN_BOUND (intmax_t) + sizeof "-%m-%d %H:%M"];

  /* Don't take the address of UT_TIME_MEMBER directly.
     Ulrich Drepper wrote:
     ``... GNU libc (and perhaps other libcs as well) have extended
     utmp file formats which do not use a simple time_t ut_time field.
     In glibc, ut_time is a macro which selects for backward compatibility
     the tv_sec member of a struct timeval value.''  */
  time_t t = UT_TIME_MEMBER (utmp_ent);
  struct tm *tmp = localtime (&t);

  if (tmp)
    {
      strftime (buf, sizeof buf, time_format, tmp);
      return buf;
    }
  else
    return timetostr (t, buf);
}

