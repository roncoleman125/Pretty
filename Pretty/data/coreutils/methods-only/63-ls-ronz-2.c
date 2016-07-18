/* Return the number of columns that format_user_or_group will print.  */

static int
format_user_or_group_width (char const *name, unsigned long int id)
{
  if (name)
    {
      int len = mbswidth (name, 0);
      return MAX (0, len);
    }
  else
    {
      char buf[INT_BUFSIZE_BOUND (id)];
      sprintf (buf, "%lu", id);
      return strlen (buf);
    }
}

