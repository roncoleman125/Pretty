/* Return true if a terminal device given as PSTAT allows other users
   to send messages to; false otherwise */
static bool
is_tty_writable (struct stat const *pstat)
{
  /* Ensure the group of the TTY device matches TTY_GROUP_NAME, more info at
     https://bugzilla.redhat.com/454261 */
  struct group *ttygr = getgrnam (TTY_GROUP_NAME);
  if (!ttygr || (pstat->st_gid != ttygr->gr_gid))
    return false;

  return pstat->st_mode & S_IWGRP;
}

