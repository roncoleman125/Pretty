/* Return true if copy of file SRC_NAME to file DEST_NAME is necessary. */
static bool
need_copy (const char *src_name, const char *dest_name,
           const struct cp_options *x)
{
  struct stat src_sb, dest_sb;
  int src_fd, dest_fd;
  bool content_match;

  if (extra_mode (mode))
    return true;

  /* compare files using stat */
  if (lstat (src_name, &src_sb) != 0)
    return true;

  if (lstat (dest_name, &dest_sb) != 0)
    return true;

  if (!S_ISREG (src_sb.st_mode) || !S_ISREG (dest_sb.st_mode)
      || extra_mode (src_sb.st_mode) || extra_mode (dest_sb.st_mode))
    return true;

  if (src_sb.st_size != dest_sb.st_size
      || (dest_sb.st_mode & CHMOD_MODE_BITS) != mode
      || dest_sb.st_uid != (owner_id == (uid_t) -1 ? getuid () : owner_id)
      || dest_sb.st_gid != (group_id == (gid_t) -1 ? getgid () : group_id))
    return true;

  /* compare SELinux context if preserving */
  if (selinux_enabled && x->preserve_security_context)
    {
      security_context_t file_scontext = NULL;
      security_context_t to_scontext = NULL;
      bool scontext_match;

      if (getfilecon (src_name, &file_scontext) == -1)
        return true;

      if (getfilecon (dest_name, &to_scontext) == -1)
        {
          freecon (file_scontext);
          return true;
        }

      scontext_match = STREQ (file_scontext, to_scontext);

      freecon (file_scontext);
      freecon (to_scontext);
      if (!scontext_match)
        return true;
    }

  /* compare files content */
  src_fd = open (src_name, O_RDONLY | O_BINARY);
  if (src_fd < 0)
    return true;

  dest_fd = open (dest_name, O_RDONLY | O_BINARY);
  if (dest_fd < 0)
    {
      close (src_fd);
      return true;
    }

  content_match = have_same_content (src_fd, dest_fd);

  close (src_fd);
  close (dest_fd);
  return !content_match;
}

