/* Add the comma or blank separated list of tab stops STOPS
   to the list of tab stops.  */

static void
parse_tab_stops (char const *stops)
{
  bool have_tabval = false;
  uintmax_t tabval IF_LINT ( = 0);
  char const *num_start IF_LINT ( = NULL);
  bool ok = true;

  for (; *stops; stops++)
    {
      if (*stops == ',' || isblank (to_uchar (*stops)))
        {
          if (have_tabval)
            add_tab_stop (tabval);
          have_tabval = false;
        }
      else if (ISDIGIT (*stops))
        {
          if (!have_tabval)
            {
              tabval = 0;
              have_tabval = true;
              num_start = stops;
            }

          /* Detect overflow.  */
          if (!DECIMAL_DIGIT_ACCUMULATE (tabval, *stops - '0', uintmax_t))
            {
              size_t len = strspn (num_start, "0123456789");
              char *bad_num = xstrndup (num_start, len);
              error (0, 0, _("tab stop is too large %s"), quote (bad_num));
              free (bad_num);
              ok = false;
              stops = num_start + len - 1;
            }
        }
      else
        {
          error (0, 0, _("tab size contains invalid character(s): %s"),
                 quote (stops));
          ok = false;
          break;
        }
    }

  if (!ok)
    exit (EXIT_FAILURE);

  if (have_tabval)
    add_tab_stop (tabval);
}

