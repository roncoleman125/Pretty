/* Print the name or value of group ID GID. */
extern bool
print_group (gid_t gid, bool use_name)
{
  struct group *grp = NULL;
  bool ok = true;

  if (use_name)
    {
      grp = getgrgid (gid);
      if (grp == NULL)
        {
          error (0, 0, _("cannot find name for group ID %lu"),
                 (unsigned long int) gid);
          ok = false;
        }
    }

  if (grp == NULL)
    printf ("%lu", (unsigned long int) gid);
  else
    printf ("%s", grp->gr_name);
  return ok;
}
