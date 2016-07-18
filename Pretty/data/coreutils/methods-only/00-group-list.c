/* Print all of the distinct groups the user is in. */
extern bool
print_group_list (const char *username,
                  uid_t ruid, gid_t rgid, gid_t egid,
                  bool use_names)
{
  bool ok = true;
  struct passwd *pwd;

  pwd = getpwuid (ruid);
  if (pwd == NULL)
    ok = false;

  if (!print_group (rgid, use_names))
    ok = false;

  if (egid != rgid)
    {
      putchar (' ');
      if (!print_group (egid, use_names))
        ok = false;
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
        return false;
      }

    for (i = 0; i < n_groups; i++)
      if (groups[i] != rgid && groups[i] != egid)
        {
          putchar (' ');
          if (!print_group (groups[i], use_names))
            ok = false;
        }
    free (groups);
  }
  return ok;
}


