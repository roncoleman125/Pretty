/* Print the name or value of user ID UID. */

static void
print_user (uid_t uid)
{
  struct passwd *pwd = NULL;

  if (use_name)
    {
      pwd = getpwuid (uid);
      if (pwd == NULL)
        {
          error (0, 0, _("cannot find name for user ID %lu"),
                 (unsigned long int) uid);
          ok = false;
        }
    }

  if (pwd == NULL)
    printf ("%lu", (unsigned long int) uid);
  else
    printf ("%s", pwd->pw_name);
}

