/* Display UTMP_BUF, which should have N entries. */

static void
scan_entries (size_t n, const STRUCT_UTMP *utmp_buf,
              const int argc_names, char *const argv_names[])
{
  if (hard_locale (LC_TIME))
    {
      time_format = "%Y-%m-%d %H:%M";
      time_format_width = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2;
    }
  else
    {
      time_format = "%b %e %H:%M";
      time_format_width = 3 + 1 + 2 + 1 + 2 + 1 + 2;
    }

  if (include_heading)
    print_heading ();

  while (n--)
    {
      if (IS_USER_PROCESS (utmp_buf))
        {
          if (argc_names)
            {
              int i;

              for (i = 0; i < argc_names; i++)
                if (strncmp (UT_USER (utmp_buf), argv_names[i], UT_USER_SIZE)
                    == 0)
                  {
                    print_entry (utmp_buf);
                    break;
                  }
            }
          else
            print_entry (utmp_buf);
        }
      utmp_buf++;
    }
}

