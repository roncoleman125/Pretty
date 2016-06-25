/* Return any bind mounted source for a path.
   The caller should not free the returned buffer.
   Return NULL if no bind mount found.  */
static char const * ATTRIBUTE_WARN_UNUSED_RESULT
find_bind_mount (char const * name)
{
  char const * bind_mount = NULL;

  static struct mount_entry *mount_list;
  static bool tried_mount_list = false;
  if (!tried_mount_list) /* attempt/warn once per process.  */
    {
      if (!(mount_list = read_file_system_list (false)))
        error (0, errno, "%s", _("cannot read table of mounted file systems"));
      tried_mount_list = true;
    }

  struct mount_entry *me;
  for (me = mount_list; me; me = me->me_next)
    {
      if (me->me_dummy && me->me_devname[0] == '/'
          && STREQ (me->me_mountdir, name))
        {
          struct stat name_stats;
          struct stat dev_stats;

          if (stat (name, &name_stats) == 0
              && stat (me->me_devname, &dev_stats) == 0
              && SAME_INODE (name_stats, dev_stats))
            {
              bind_mount = me->me_devname;
              break;
            }
        }
    }

  return bind_mount;
}

