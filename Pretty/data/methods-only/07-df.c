/* If DISK corresponds to a mount point, show its usage
   and return true.  Otherwise, return false.  */
static bool
show_disk (char const *disk)
{
  struct mount_entry const *me;
  struct mount_entry const *best_match = NULL;

  for (me = mount_list; me; me = me->me_next)
    if (STREQ (disk, me->me_devname))
      best_match = me;

  if (best_match)
    {
      show_dev (best_match->me_devname, best_match->me_mountdir, NULL,
                best_match->me_type, best_match->me_dummy,
                best_match->me_remote, NULL);
      return true;
    }

  return false;
}

