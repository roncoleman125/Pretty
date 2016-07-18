/* Initialize the user and group ownership of the files to install. */

static void
get_ids (void)
{
  struct passwd *pw;
  struct group *gr;

  if (owner_name)
    {
      pw = getpwnam (owner_name);
      if (pw == NULL)
        {
          unsigned long int tmp;
          if (xstrtoul (owner_name, NULL, 0, &tmp, NULL) != LONGINT_OK
              || UID_T_MAX < tmp)
            error (EXIT_FAILURE, 0, _("invalid user %s"), quote (owner_name));
          owner_id = tmp;
        }
      else
        owner_id = pw->pw_uid;
      endpwent ();
    }
  else
    owner_id = (uid_t) -1;

  if (group_name)
    {
      gr = getgrnam (group_name);
      if (gr == NULL)
        {
          unsigned long int tmp;
          if (xstrtoul (group_name, NULL, 0, &tmp, NULL) != LONGINT_OK
              || GID_T_MAX < tmp)
            error (EXIT_FAILURE, 0, _("invalid group %s"), quote (group_name));
          group_id = tmp;
        }
      else
        group_id = gr->gr_gid;
      endgrent ();
    }
  else
    group_id = (gid_t) -1;
}

