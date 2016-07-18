static int
xstrcoll (char const *a, char const *b)
{
  int diff;
  errno = 0;
  diff = strcoll (a, b);
  if (errno)
    {
      error (0, errno, _("cannot compare file names %s and %s"),
             quote_n (0, a), quote_n (1, b));
      set_exit_status (false);
      longjmp (failed_strcoll, 1);
    }
  return diff;
}

/* Comparison routines for sorting the files.  */
