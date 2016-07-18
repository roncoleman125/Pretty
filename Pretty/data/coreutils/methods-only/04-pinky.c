/* Display a line of information about UTMP_ENT. */

static void
print_entry (const STRUCT_UTMP *utmp_ent)
{
  struct stat stats;
  time_t last_change;
  char mesg;


  char line[sizeof (utmp_ent->ut_line) + DEV_DIR_LEN + 1];

  /* Copy ut_line into LINE, prepending `/dev/' if ut_line is not
     already an absolute file name.  Some system may put the full,
     absolute file name in ut_line.  */
  if (utmp_ent->ut_line[0] == '/')
    {
      strncpy (line, utmp_ent->ut_line, sizeof (utmp_ent->ut_line));
      line[sizeof (utmp_ent->ut_line)] = '\0';
    }
  else
    {
      strcpy (line, DEV_DIR_WITH_TRAILING_SLASH);
      strncpy (line + DEV_DIR_LEN, utmp_ent->ut_line, sizeof utmp_ent->ut_line);
      line[DEV_DIR_LEN + sizeof (utmp_ent->ut_line)] = '\0';
    }

  if (stat (line, &stats) == 0)
    {
      mesg = (stats.st_mode & S_IWGRP) ? ' ' : '*';
      last_change = stats.st_atime;
    }
  else
    {
      mesg = '?';
      last_change = 0;
    }

  printf ("%-8.*s", UT_USER_SIZE, UT_USER (utmp_ent));

  if (include_fullname)
    {
      struct passwd *pw;
      char name[UT_USER_SIZE + 1];

      strncpy (name, UT_USER (utmp_ent), UT_USER_SIZE);
      name[UT_USER_SIZE] = '\0';
      pw = getpwnam (name);
      if (pw == NULL)
        /* TRANSLATORS: Real name is unknown; at most 19 characters. */
        printf (" %19s", _("        ???"));
      else
        {
          char *const comma = strchr (pw->pw_gecos, ',');
          char *result;

          if (comma)
            *comma = '\0';

          result = create_fullname (pw->pw_gecos, pw->pw_name);
          printf (" %-19.19s", result);
          free (result);
        }
    }

  printf (" %c%-8.*s",
          mesg, (int) sizeof (utmp_ent->ut_line), utmp_ent->ut_line);

  if (include_idle)
    {
      if (last_change)
        printf (" %-6s", idle_string (last_change));
      else
        /* TRANSLATORS: Idle time is unknown; at most 5 characters. */
        printf (" %-6s", _("?????"));
    }

  printf (" %s", time_string (utmp_ent));

  if (include_where && utmp_ent->ut_host[0])
    {
      char ut_host[sizeof (utmp_ent->ut_host) + 1];
      char *host = NULL;
      char *display = NULL;

      /* Copy the host name into UT_HOST, and ensure it's nul terminated. */
      strncpy (ut_host, utmp_ent->ut_host, (int) sizeof (utmp_ent->ut_host));
      ut_host[sizeof (utmp_ent->ut_host)] = '\0';

      /* Look for an X display.  */
      display = strchr (ut_host, ':');
      if (display)
        *display++ = '\0';

      if (*ut_host)
        /* See if we can canonicalize it.  */
        host = canon_host (ut_host);
      if ( ! host)
        host = ut_host;

      if (display)
        printf (" %s:%s", host, display);
      else
        printf (" %s", host);

      if (host != ut_host)
        free (host);
    }

  putchar ('\n');
}

