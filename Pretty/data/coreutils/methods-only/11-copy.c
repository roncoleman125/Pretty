/* Copy a regular file from SRC_NAME to DST_NAME.
   If the source file contains holes, copies holes and blocks of zeros
   in the source file as holes in the destination file.
   (Holes are read as zeroes by the `read' system call.)
   When creating the destination, use DST_MODE & ~OMITTED_PERMISSIONS
   as the third argument in the call to open, adding
   OMITTED_PERMISSIONS after copying as needed.
   X provides many option settings.
   Return true if successful.
   *NEW_DST is as in copy_internal.
   SRC_SB is the result of calling XSTAT (aka stat) on SRC_NAME.  */

static bool
copy_reg (char const *src_name, char const *dst_name,
          const struct cp_options *x,
          mode_t dst_mode, mode_t omitted_permissions, bool *new_dst,
          struct stat const *src_sb)
{
  char *buf;
  char *buf_alloc = NULL;
  char *name_alloc = NULL;
  int dest_desc;
  int dest_errno;
  int source_desc;
  mode_t src_mode = src_sb->st_mode;
  struct stat sb;
  struct stat src_open_sb;
  bool return_val = true;
  bool data_copy_required = x->data_copy_required;

  source_desc = open (src_name,
                      (O_RDONLY | O_BINARY
                       | (x->dereference == DEREF_NEVER ? O_NOFOLLOW : 0)));
  if (source_desc < 0)
    {
      error (0, errno, _("cannot open %s for reading"), quote (src_name));
      return false;
    }

  if (fstat (source_desc, &src_open_sb) != 0)
    {
      error (0, errno, _("cannot fstat %s"), quote (src_name));
      return_val = false;
      goto close_src_desc;
    }

  /* Compare the source dev/ino from the open file to the incoming,
     saved ones obtained via a previous call to stat.  */
  if (! SAME_INODE (*src_sb, src_open_sb))
    {
      error (0, 0,
             _("skipping file %s, as it was replaced while being copied"),
             quote (src_name));
      return_val = false;
      goto close_src_desc;
    }

  /* The semantics of the following open calls are mandated
     by the specs for both cp and mv.  */
  if (! *new_dst)
    {
      dest_desc = open (dst_name, O_WRONLY | O_TRUNC | O_BINARY);
      dest_errno = errno;

      /* When using cp --preserve=context to copy to an existing destination,
         use the default context rather than that of the source.  Why?
         1) the src context may prohibit writing, and
         2) because it's more consistent to use the same context
         that is used when the destination file doesn't already exist.  */
      if (x->preserve_security_context && 0 <= dest_desc)
        {
          bool all_errors = (!x->data_copy_required
                             || x->require_preserve_context);
          bool some_errors = !all_errors && !x->reduce_diagnostics;
          security_context_t con = NULL;

          if (getfscreatecon (&con) < 0)
            {
              if (all_errors || (some_errors && !errno_unsupported (errno)))
                error (0, errno, _("failed to get file system create context"));
              if (x->require_preserve_context)
                {
                  return_val = false;
                  goto close_src_and_dst_desc;
                }
            }

          if (con)
            {
              if (fsetfilecon (dest_desc, con) < 0)
                {
                  if (all_errors || (some_errors && !errno_unsupported (errno)))
                    error (0, errno,
                           _("failed to set the security context of %s to %s"),
                           quote_n (0, dst_name), quote_n (1, con));
                  if (x->require_preserve_context)
                    {
                      return_val = false;
                      freecon (con);
                      goto close_src_and_dst_desc;
                    }
                }
              freecon (con);
            }
        }

      if (dest_desc < 0 && x->unlink_dest_after_failed_open)
        {
          if (unlink (dst_name) != 0)
            {
              error (0, errno, _("cannot remove %s"), quote (dst_name));
              return_val = false;
              goto close_src_desc;
            }
          if (x->verbose)
            printf (_("removed %s\n"), quote (dst_name));

          /* Tell caller that the destination file was unlinked.  */
          *new_dst = true;
        }
    }

  if (*new_dst)
    {
      int open_flags = O_WRONLY | O_CREAT | O_BINARY;
      dest_desc = open (dst_name, open_flags | O_EXCL,
                        dst_mode & ~omitted_permissions);
      dest_errno = errno;

      /* When trying to copy through a dangling destination symlink,
         the above open fails with EEXIST.  If that happens, and
         lstat'ing the DST_NAME shows that it is a symlink, then we
         have a problem: trying to resolve this dangling symlink to
         a directory/destination-entry pair is fundamentally racy,
         so punt.  If x->open_dangling_dest_symlink is set (cp sets
         that when POSIXLY_CORRECT is set in the environment), simply
         call open again, but without O_EXCL (potentially dangerous).
         If not, fail with a diagnostic.  These shenanigans are necessary
         only when copying, i.e., not in move_mode.  */
      if (dest_desc < 0 && dest_errno == EEXIST && ! x->move_mode)
        {
          struct stat dangling_link_sb;
          if (lstat (dst_name, &dangling_link_sb) == 0
              && S_ISLNK (dangling_link_sb.st_mode))
            {
              if (x->open_dangling_dest_symlink)
                {
                  dest_desc = open (dst_name, open_flags,
                                    dst_mode & ~omitted_permissions);
                  dest_errno = errno;
                }
              else
                {
                  error (0, 0, _("not writing through dangling symlink %s"),
                         quote (dst_name));
                  return_val = false;
                  goto close_src_desc;
                }
            }
        }

      /* Improve quality of diagnostic when a nonexistent dst_name
         ends in a slash and open fails with errno == EISDIR.  */
      if (dest_desc < 0 && dest_errno == EISDIR
          && *dst_name && dst_name[strlen (dst_name) - 1] == '/')
        dest_errno = ENOTDIR;
    }
  else
    omitted_permissions = 0;

  if (dest_desc < 0)
    {
      error (0, dest_errno, _("cannot create regular file %s"),
             quote (dst_name));
      return_val = false;
      goto close_src_desc;
    }

  if (fstat (dest_desc, &sb) != 0)
    {
      error (0, errno, _("cannot fstat %s"), quote (dst_name));
      return_val = false;
      goto close_src_and_dst_desc;
    }

  /* --attributes-only overrides --reflink.  */
  if (data_copy_required && x->reflink_mode)
    {
      bool clone_ok = clone_file (dest_desc, source_desc) == 0;
      if (clone_ok || x->reflink_mode == REFLINK_ALWAYS)
        {
          if (!clone_ok)
            {
              error (0, errno, _("failed to clone %s"), quote (dst_name));
              return_val = false;
              goto close_src_and_dst_desc;
            }
          data_copy_required = false;
        }
    }

  if (data_copy_required)
    {
      typedef uintptr_t word;

      /* Choose a suitable buffer size; it may be adjusted later.  */
      size_t buf_alignment = lcm (getpagesize (), sizeof (word));
      size_t buf_alignment_slop = sizeof (word) + buf_alignment - 1;
      size_t buf_size = io_blksize (sb);

      /* Deal with sparse files.  */
      bool make_holes = false;

      if (S_ISREG (sb.st_mode))
        {
          /* Even with --sparse=always, try to create holes only
             if the destination is a regular file.  */
          if (x->sparse_mode == SPARSE_ALWAYS)
            make_holes = true;

          /* Use a heuristic to determine whether SRC_NAME contains any sparse
             blocks.  If the file has fewer blocks than would normally be
             needed for a file of its size, then at least one of the blocks in
             the file is a hole.  */
          if (x->sparse_mode == SPARSE_AUTO && S_ISREG (src_open_sb.st_mode)
              && ST_NBLOCKS (src_open_sb) < src_open_sb.st_size / ST_NBLOCKSIZE)
            make_holes = true;
        }

      /* If not making a sparse file, try to use a more-efficient
         buffer size.  */
      if (! make_holes)
        {
          /* Compute the least common multiple of the input and output
             buffer sizes, adjusting for outlandish values.  */
          size_t blcm_max = MIN (SIZE_MAX, SSIZE_MAX) - buf_alignment_slop;
          size_t blcm = buffer_lcm (io_blksize (src_open_sb), buf_size,
                                    blcm_max);

          /* Do not bother with a buffer larger than the input file, plus one
             byte to make sure the file has not grown while reading it.  */
          if (S_ISREG (src_open_sb.st_mode) && src_open_sb.st_size < buf_size)
            buf_size = src_open_sb.st_size + 1;

          /* However, stick with a block size that is a positive multiple of
             blcm, overriding the above adjustments.  Watch out for
             overflow.  */
          buf_size += blcm - 1;
          buf_size -= buf_size % blcm;
          if (buf_size == 0 || blcm_max < buf_size)
            buf_size = blcm;
        }

      /* Make a buffer with space for a sentinel at the end.  */
      buf_alloc = xmalloc (buf_size + buf_alignment_slop);
      buf = ptr_align (buf_alloc, buf_alignment);

      bool normal_copy_required;
      /* Perform an efficient extent-based copy, falling back to the
         standard copy only if the initial extent scan fails.  If the
         '--sparse=never' option is specified, write all data but use
         any extents to read more efficiently.  */
      if (extent_copy (source_desc, dest_desc, buf, buf_size,
                       src_open_sb.st_size, make_holes,
                       src_name, dst_name, &normal_copy_required))
        goto preserve_metadata;

      if (! normal_copy_required)
        {
          return_val = false;
          goto close_src_and_dst_desc;
        }

      off_t n_read;
      bool wrote_hole_at_eof;
      if ( ! sparse_copy (source_desc, dest_desc, buf, buf_size,
                          make_holes, src_name, dst_name,
                          UINTMAX_MAX, &n_read,
                          &wrote_hole_at_eof)
           || (wrote_hole_at_eof &&
               ! sparse_copy_finalize (dest_desc, dst_name)))
        {
          return_val = false;
          goto close_src_and_dst_desc;
        }
    }

preserve_metadata:
  if (x->preserve_timestamps)
    {
      struct timespec timespec[2];
      timespec[0] = get_stat_atime (src_sb);
      timespec[1] = get_stat_mtime (src_sb);

      if (fdutimens (dest_desc, dst_name, timespec) != 0)
        {
          error (0, errno, _("preserving times for %s"), quote (dst_name));
          if (x->require_preserve)
            {
              return_val = false;
              goto close_src_and_dst_desc;
            }
        }
    }

  /* Set ownership before xattrs as changing owners will
     clear capabilities.  */
  if (x->preserve_ownership && ! SAME_OWNER_AND_GROUP (*src_sb, sb))
    {
      switch (set_owner (x, dst_name, dest_desc, src_sb, *new_dst, &sb))
        {
        case -1:
          return_val = false;
          goto close_src_and_dst_desc;

        case 0:
          src_mode &= ~ (S_ISUID | S_ISGID | S_ISVTX);
          break;
        }
    }

  /* To allow copying xattrs on read-only files, temporarily chmod u+rw.
     This workaround is required as an inode permission check is done
     by xattr_permission() in fs/xattr.c of the GNU/Linux kernel tree.  */
  if (x->preserve_xattr)
    {
      bool access_changed = false;

      if (!(sb.st_mode & S_IWUSR) && geteuid () != 0)
        access_changed = fchmod_or_lchmod (dest_desc, dst_name, 0600) == 0;

      if (!copy_attr (src_name, source_desc, dst_name, dest_desc, x)
          && x->require_preserve_xattr)
        return_val = false;

      if (access_changed)
        fchmod_or_lchmod (dest_desc, dst_name, dst_mode & ~omitted_permissions);
    }

  set_author (dst_name, dest_desc, src_sb);

  if (x->preserve_mode || x->move_mode)
    {
      if (copy_acl (src_name, source_desc, dst_name, dest_desc, src_mode) != 0
          && x->require_preserve)
        return_val = false;
    }
  else if (x->set_mode)
    {
      if (set_acl (dst_name, dest_desc, x->mode) != 0)
        return_val = false;
    }
  else if (omitted_permissions)
    {
      omitted_permissions &= ~ cached_umask ();
      if (omitted_permissions
          && fchmod_or_lchmod (dest_desc, dst_name, dst_mode) != 0)
        {
          error (0, errno, _("preserving permissions for %s"),
                 quote (dst_name));
          if (x->require_preserve)
            return_val = false;
        }
    }

close_src_and_dst_desc:
  if (close (dest_desc) < 0)
    {
      error (0, errno, _("closing %s"), quote (dst_name));
      return_val = false;
    }
close_src_desc:
  if (close (source_desc) < 0)
    {
      error (0, errno, _("closing %s"), quote (src_name));
      return_val = false;
    }

  free (buf_alloc);
  free (name_alloc);
  return return_val;
}

