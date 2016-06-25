/* Print all of the info about the user's user and group IDs. */

static void
print_full_info (const char *username)
{
  struct passwd *pwd;
  struct group *grp;

  printf (_("uid=%lu"), (unsigned long int) ruid);
  pwd = getpwuid (ruid);
  if (pwd)
    printf ("(%s)", pwd->pw_name);

  printf (_(" gid=%lu"), (unsigned long int) rgid);
  grp = getgrgid (rgid);
  if (grp)
    printf ("(%s)", grp->gr_name);

  if (euid != ruid)
    {
      printf (_(" euid=%lu"), (unsigned long int) euid);
      pwd = getpwuid (euid);
      if (pwd)
        printf ("(%s)", pwd->pw_name);
    }

  if (egid != rgid)
    {
      printf (_(" egid=%lu"), (unsigned long int) egid);
      grp = getgrgid (egid);
      if (grp)
        printf ("(%s)", grp->gr_name);
    }

  {
    gid_t *groups;
    int i;

    int n_groups = xgetgroups (username, (pwd ? pwd->pw_gid : (gid_t) -1),
                               &groups);
    if (n_groups < 0)
      {
        if (username)
          {
            error (0, errno, _("failed to get groups for user %s"),
                   quote (username));
          }
        else
          {
            error (0, errno, _("failed to get groups for the current process"));
          }
        ok = false;
        return;
      }

    if (n_groups > 0)
      fputs (_(" groups="), stdout);
    for (i = 0; i < n_groups; i++)
      {
        if (i > 0)
          putchar (',');
        printf ("%lu", (unsigned long int) groups[i]);
        grp = getgrgid (groups[i]);
        if (grp)
          printf ("(%s)", grp->gr_name);
      }
    free (groups);
  }

  /* POSIX mandates the precise output format, and that it not include
     any context=... part, so skip that if POSIXLY_CORRECT is set.  */
  if (context != NULL && ! getenv ("POSIXLY_CORRECT"))
    printf (_(" context=%s"), context);
}
