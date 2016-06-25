/* Set the owner and owning group of DEST_DESC to the st_uid and
   st_gid fields of SRC_SB.  If DEST_DESC is undefined (-1), set
   the owner and owning group of DST_NAME instead; for
   safety prefer lchown if the system supports it since no
   symbolic links should be involved.  DEST_DESC must
   refer to the same file as DEST_NAME if defined.
   Upon failure to set both UID and GID, try to set only the GID.
   NEW_DST is true if the file was newly created; otherwise,
   DST_SB is the status of the destination.
   Return 1 if the initial syscall succeeds, 0 if it fails but it's OK
   not to preserve ownership, -1 otherwise.  */

static int
set_owner (const struct cp_options *x, char const *dst_name, int dest_desc,
           struct stat const *src_sb, bool new_dst,
           struct stat const *dst_sb)
{
  uid_t uid = src_sb->st_uid;
  gid_t gid = src_sb->st_gid;

  /* Naively changing the ownership of an already-existing file before
     changing its permissions would create a window of vulnerability if
     the file's old permissions are too generous for the new owner and
     group.  Avoid the window by first changing to a restrictive
     temporary mode if necessary.  */

  if (!new_dst && (x->preserve_mode || x->move_mode || x->set_mode))
    {
      mode_t old_mode = dst_sb->st_mode;
      mode_t new_mode =
        (x->preserve_mode || x->move_mode ? src_sb->st_mode : x->mode);
      mode_t restrictive_temp_mode = old_mode & new_mode & S_IRWXU;

      if ((USE_ACL
           || (old_mode & CHMOD_MODE_BITS
               & (~new_mode | S_ISUID | S_ISGID | S_ISVTX)))
          && qset_acl (dst_name, dest_desc, restrictive_temp_mode) != 0)
        {
          if (! owner_failure_ok (x))
            error (0, errno, _("clearing permissions for %s"),
                   quote (dst_name));
          return -x->require_preserve;
        }
    }

  if (HAVE_FCHOWN && dest_desc != -1)
    {
      if (fchown (dest_desc, uid, gid) == 0)
        return 1;
      if (errno == EPERM || errno == EINVAL)
        {
          /* We've failed to set *both*.  Now, try to set just the group
             ID, but ignore any failure here, and don't change errno.  */
          int saved_errno = errno;
          ignore_value (fchown (dest_desc, -1, gid));
          errno = saved_errno;
        }
    }
  else
    {
      if (lchown (dst_name, uid, gid) == 0)
        return 1;
      if (errno == EPERM || errno == EINVAL)
        {
          /* We've failed to set *both*.  Now, try to set just the group
             ID, but ignore any failure here, and don't change errno.  */
          int saved_errno = errno;
          ignore_value (lchown (dst_name, -1, gid));
          errno = saved_errno;
        }
    }

  if (! chown_failure_ok (x))
    {
      error (0, errno, _("failed to preserve ownership for %s"),
             quote (dst_name));
      if (x->require_preserve)
        return -1;
    }

  return 0;
}

