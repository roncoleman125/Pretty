/* Return the group ID of NAME, or -1 if no name was specified.  */

static gid_t
parse_group (const char *name)
{
  gid_t gid = -1;

  if (*name)
    {
      struct group *grp = getgrnam (name);
      if (grp)
        gid = grp->gr_gid;
      else
        {
          unsigned long int tmp;
          if (! (xstrtoul (name, NULL, 10, &tmp, "") == LONGINT_OK
                 && tmp <= GID_T_MAX))
            error (EXIT_FAILURE, 0, _("invalid group: %s"), quote (name));
          gid = tmp;
        }
      endgrent ();		/* Save a file descriptor. */
    }

  return gid;
}

