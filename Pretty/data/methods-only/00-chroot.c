/* Call setgroups to set the supplementary groups to those listed in GROUPS.
   GROUPS is a comma separated list of supplementary groups (names or numbers).
   Parse that list, converting any names to numbers, and call setgroups on the
   resulting numbers.  Upon any failure give a diagnostic and return nonzero.
   Otherwise return zero.  */
static int
set_additional_groups (char const *groups)
{
  GETGROUPS_T *gids = NULL;
  size_t n_gids_allocated = 0;
  size_t n_gids = 0;
  char *buffer = xstrdup (groups);
  char const *tmp;
  int ret = 0;

  for (tmp = strtok (buffer, ","); tmp; tmp = strtok (NULL, ","))
    {
      struct group *g;
      unsigned long int value;

      if (xstrtoul (tmp, NULL, 10, &value, "") == LONGINT_OK && value <= MAXGID)
        g = getgrgid (value);
      else
        {
          g = getgrnam (tmp);
          if (g != NULL)
            value = g->gr_gid;
        }

      if (g == NULL)
        {
          error (0, errno, _("invalid group %s"), quote (tmp));
          ret = -1;
          continue;
        }

      if (n_gids == n_gids_allocated)
        gids = X2NREALLOC (gids, &n_gids_allocated);
      gids[n_gids++] = value;
    }

  if (ret == 0 && n_gids == 0)
    {
      error (0, 0, _("invalid group list %s"), quote (groups));
      ret = -1;
    }

  if (ret == 0)
    {
      ret = setgroups (n_gids, gids);
      if (ret)
        error (0, errno, _("failed to set additional groups"));
    }

  free (buffer);
  free (gids);
  return ret;
}

