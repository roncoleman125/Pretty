/* Display UTMP_BUF, which should have N entries. */
static void
scan_entries (size_t n, const STRUCT_UTMP *utmp_buf)
{
  char *ttyname_b IF_LINT ( = NULL);
  time_t boottime = TYPE_MINIMUM (time_t);

  if (include_heading)
    print_heading ();

  if (my_line_only)
    {
      ttyname_b = ttyname (STDIN_FILENO);
      if (!ttyname_b)
        return;
      if (strncmp (ttyname_b, DEV_DIR_WITH_TRAILING_SLASH, DEV_DIR_LEN) == 0)
        ttyname_b += DEV_DIR_LEN;	/* Discard /dev/ prefix.  */
    }

  while (n--)
    {
      if (!my_line_only ||
          strncmp (ttyname_b, utmp_buf->ut_line,
                   sizeof (utmp_buf->ut_line)) == 0)
        {
          if (need_users && IS_USER_PROCESS (utmp_buf))
            print_user (utmp_buf, boottime);
          else if (need_runlevel && UT_TYPE_RUN_LVL (utmp_buf))
            print_runlevel (utmp_buf);
          else if (need_boottime && UT_TYPE_BOOT_TIME (utmp_buf))
            print_boottime (utmp_buf);
          /* I've never seen one of these, so I don't know what it should
             look like :^)
             FIXME: handle OLD_TIME also, perhaps show the delta? */
          else if (need_clockchange && UT_TYPE_NEW_TIME (utmp_buf))
            print_clockchange (utmp_buf);
          else if (need_initspawn && UT_TYPE_INIT_PROCESS (utmp_buf))
            print_initspawn (utmp_buf);
          else if (need_login && UT_TYPE_LOGIN_PROCESS (utmp_buf))
            print_login (utmp_buf);
          else if (need_deadprocs && UT_TYPE_DEAD_PROCESS (utmp_buf))
            print_deadprocs (utmp_buf);
        }

      if (UT_TYPE_BOOT_TIME (utmp_buf))
        boottime = UT_TIME_MEMBER (utmp_buf);

      utmp_buf++;
    }
}

