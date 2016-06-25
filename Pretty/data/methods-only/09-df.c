/* Determine what kind of node NAME is and show the disk usage
   for it.  STATP is the results of `stat' on NAME.  */

static void
show_entry (char const *name, struct stat const *statp)
{
  if ((S_ISBLK (statp->st_mode) || S_ISCHR (statp->st_mode))
      && show_disk (name))
    return;

  show_point (name, statp);
}

/* Show all mounted file systems, except perhaps those that are of
   an unselected type or are empty. */

static void
show_all_entries (void)
{
  struct mount_entry *me;

  for (me = mount_list; me; me = me->me_next)
    show_dev (me->me_devname, me->me_mountdir, NULL, me->me_type,
              me->me_dummy, me->me_remote, NULL);
}

