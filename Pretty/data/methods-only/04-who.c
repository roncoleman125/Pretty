/* Send properly parsed USER_PROCESS info to print_line.  The most
   recent boot time is BOOTTIME. */
static void
print_user (const STRUCT_UTMP *utmp_ent, time_t boottime)
{
  struct stat stats;
  time_t last_change;
  char mesg;
  char idlestr[IDLESTR_LEN + 1];
  static char *hoststr;
  static size_t hostlen;


  char line[sizeof (utmp_ent->ut_line) + DEV_DIR_LEN + 1];
  PIDSTR_DECL_AND_INIT (pidstr, utmp_ent);

  /* Copy ut_line into LINE, prepending `/dev/' if ut_line is not
     already an absolute file name.  Some systems may put the full,
     absolute file name in ut_line.  */
  if (utmp_ent->ut_line[0] == '/')
    {
      strncpy (line, utmp_ent->ut_line, sizeof (utmp_ent->ut_line));
      line[sizeof (utmp_ent->ut_line)] = '\0';
    }
  else
    {
      strcpy (line, DEV_DIR_WITH_TRAILING_SLASH);
      strncpy (line + DEV_DIR_LEN, utmp_ent->ut_line,
               sizeof (utmp_ent->ut_line));
      line[DEV_DIR_LEN + sizeof (utmp_ent->ut_line)] = '\0';
    }

  if (stat (line, &stats) == 0)
    {
      mesg = is_tty_writable (&stats) ? '+' : '-';
      last_change = stats.st_atime;
    }
  else
    {
      mesg = '?';
      last_change = 0;
    }

  if (last_change)
    sprintf (idlestr, "%.*s", IDLESTR_LEN, idle_string (last_change, boottime));
  else
    sprintf (idlestr, "  ?");

  if (utmp_ent->ut_host[0])
    {
      char ut_host[sizeof (utmp_ent->ut_host) + 1];
      char *host = NULL;
      char *display = NULL;

      /* Copy the host name into UT_HOST, and ensure it's nul terminated. */
      strncpy (ut_host, utmp_ent->ut_host, sizeof (utmp_ent->ut_host));
      ut_host[sizeof (utmp_ent->ut_host)] = '\0';

      /* Look for an X display.  */
      display = strchr (ut_host, ':');
      if (display)
        *display++ = '\0';

      if (*ut_host && do_lookup)
        {
          /* See if we can canonicalize it.  */
          host = canon_host (ut_host);
        }

      if (! host)
        host = ut_host;

      if (display)
        {
          if (hostlen < strlen (host) + strlen (display) + 4)
            {
              hostlen = strlen (host) + strlen (display) + 4;
              free (hoststr);
              hoststr = xmalloc (hostlen);
            }
          sprintf (hoststr, "(%s:%s)", host, display);
        }
      else
        {
          if (hostlen < strlen (host) + 3)
            {
              hostlen = strlen (host) + 3;
              free (hoststr);
              hoststr = xmalloc (hostlen);
            }
          sprintf (hoststr, "(%s)", host);
        }

      if (host != ut_host)
        free (host);
    }
  else
    {
      if (hostlen < 1)
        {
          hostlen = 1;
          free (hoststr);
          hoststr = xmalloc (hostlen);
        }
      *hoststr = '\0';
    }

  print_line (sizeof UT_USER (utmp_ent), UT_USER (utmp_ent), mesg,
              sizeof utmp_ent->ut_line, utmp_ent->ut_line,
              time_string (utmp_ent), idlestr, pidstr,
              hoststr ? hoststr : "", "");
}

