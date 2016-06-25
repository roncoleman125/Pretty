/* Ensure that the parent directory of CONST_DIR exists, for
   the --parents option.

   SRC_OFFSET is the index in CONST_DIR (which is a destination
   directory) of the beginning of the source directory name.
   Create any leading directories that don't already exist.
   If VERBOSE_FMT_STRING is nonzero, use it as a printf format
   string for printing a message after successfully making a directory.
   The format should take two string arguments: the names of the
   source and destination directories.
   Creates a linked list of attributes of intermediate directories,
   *ATTR_LIST, for re_protect to use after calling copy.
   Sets *NEW_DST if this function creates parent of CONST_DIR.

   Return true if parent of CONST_DIR exists as a directory with the proper
   permissions when done.  */

/* FIXME: Synch this function with the one in ../lib/mkdir-p.c.  */

static bool
make_dir_parents_private (char const *const_dir, size_t src_offset,
                          char const *verbose_fmt_string,
                          struct dir_attr **attr_list, bool *new_dst,
                          const struct cp_options *x)
{
  struct stat stats;
  char *dir;		/* A copy of CONST_DIR we can change.  */
  char *src;		/* Source name in DIR.  */
  char *dst_dir;	/* Leading directory of DIR.  */
  size_t dirlen;	/* Length of DIR.  */

  ASSIGN_STRDUPA (dir, const_dir);

  src = dir + src_offset;

  dirlen = dir_len (dir);
  dst_dir = alloca (dirlen + 1);
  memcpy (dst_dir, dir, dirlen);
  dst_dir[dirlen] = '\0';

  *attr_list = NULL;

  if (stat (dst_dir, &stats) != 0)
    {
      /* A parent of CONST_DIR does not exist.
         Make all missing intermediate directories. */
      char *slash;

      slash = src;
      while (*slash == '/')
        slash++;
      while ((slash = strchr (slash, '/')))
        {
          struct dir_attr *new IF_LINT ( = NULL);
          bool missing_dir;

          *slash = '\0';
          missing_dir = (stat (dir, &stats) != 0);

          if (missing_dir || x->preserve_ownership || x->preserve_mode
              || x->preserve_timestamps)
            {
              /* Add this directory to the list of directories whose
                 modes might need fixing later. */
              struct stat src_st;
              int src_errno = (stat (src, &src_st) != 0
                               ? errno
                               : S_ISDIR (src_st.st_mode)
                               ? 0
                               : ENOTDIR);
              if (src_errno)
                {
                  error (0, src_errno, _("failed to get attributes of %s"),
                         quote (src));
                  return false;
                }

              new = xmalloc (sizeof *new);
              new->st = src_st;
              new->slash_offset = slash - dir;
              new->restore_mode = false;
              new->next = *attr_list;
              *attr_list = new;
            }

          if (missing_dir)
            {
              mode_t src_mode;
              mode_t omitted_permissions;
              mode_t mkdir_mode;

              /* This component does not exist.  We must set
                 *new_dst and new->st.st_mode inside this loop because,
                 for example, in the command `cp --parents ../a/../b/c e_dir',
                 make_dir_parents_private creates only e_dir/../a if
                 ./b already exists. */
              *new_dst = true;
              src_mode = new->st.st_mode;

              /* If the ownership or special mode bits might change,
                 omit some permissions at first, so unauthorized users
                 cannot nip in before the file is ready.  */
              omitted_permissions = (src_mode
                                     & (x->preserve_ownership
                                        ? S_IRWXG | S_IRWXO
                                        : x->preserve_mode
                                        ? S_IWGRP | S_IWOTH
                                        : 0));

              /* POSIX says mkdir's behavior is implementation-defined when
                 (src_mode & ~S_IRWXUGO) != 0.  However, common practice is
                 to ask mkdir to copy all the CHMOD_MODE_BITS, letting mkdir
                 decide what to do with S_ISUID | S_ISGID | S_ISVTX.  */
              mkdir_mode = src_mode & CHMOD_MODE_BITS & ~omitted_permissions;
              if (mkdir (dir, mkdir_mode) != 0)
                {
                  error (0, errno, _("cannot make directory %s"),
                         quote (dir));
                  return false;
                }
              else
                {
                  if (verbose_fmt_string != NULL)
                    printf (verbose_fmt_string, src, dir);
                }

              /* We need search and write permissions to the new directory
                 for writing the directory's contents. Check if these
                 permissions are there.  */

              if (lstat (dir, &stats))
                {
                  error (0, errno, _("failed to get attributes of %s"),
                         quote (dir));
                  return false;
                }


              if (! x->preserve_mode)
                {
                  if (omitted_permissions & ~stats.st_mode)
                    omitted_permissions &= ~ cached_umask ();
                  if (omitted_permissions & ~stats.st_mode
                      || (stats.st_mode & S_IRWXU) != S_IRWXU)
                    {
                      new->st.st_mode = stats.st_mode | omitted_permissions;
                      new->restore_mode = true;
                    }
                }

              if ((stats.st_mode & S_IRWXU) != S_IRWXU)
                {
                  /* Make the new directory searchable and writable.
                     The original permissions will be restored later.  */

                  if (lchmod (dir, stats.st_mode | S_IRWXU) != 0)
                    {
                      error (0, errno, _("setting permissions for %s"),
                             quote (dir));
                      return false;
                    }
                }
            }
          else if (!S_ISDIR (stats.st_mode))
            {
              error (0, 0, _("%s exists but is not a directory"),
                     quote (dir));
              return false;
            }
          else
            *new_dst = false;
          *slash++ = '/';

          /* Avoid unnecessary calls to `stat' when given
             file names containing multiple adjacent slashes.  */
          while (*slash == '/')
            slash++;
        }
    }

  /* We get here if the parent of DIR already exists.  */

  else if (!S_ISDIR (stats.st_mode))
    {
      error (0, 0, _("%s exists but is not a directory"), quote (dst_dir));
      return false;
    }
  else
    {
      *new_dst = false;
    }
  return true;
}

