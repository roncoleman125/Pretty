/* Copy the file SRC_NAME to the file DST_NAME.  The files may be of
   any type.  NEW_DST should be true if the file DST_NAME cannot
   exist because its parent directory was just created; NEW_DST should
   be false if DST_NAME might already exist.  DEVICE is the device
   number of the parent directory, or 0 if the parent of this file is
   not known.  ANCESTORS points to a linked, null terminated list of
   devices and inodes of parent directories of SRC_NAME.  COMMAND_LINE_ARG
   is true iff SRC_NAME was specified on the command line.
   FIRST_DIR_CREATED_PER_COMMAND_LINE_ARG is both input and output.
   Set *COPY_INTO_SELF if SRC_NAME is a parent of (or the
   same as) DST_NAME; otherwise, clear it.
   Return true if successful.  */
static bool
copy_internal (char const *src_name, char const *dst_name,
               bool new_dst,
               dev_t device,
               struct dir_list *ancestors,
               const struct cp_options *x,
               bool command_line_arg,
               bool *first_dir_created_per_command_line_arg,
               bool *copy_into_self,
               bool *rename_succeeded)
{
  struct stat src_sb;
  struct stat dst_sb;
  mode_t src_mode;
  mode_t dst_mode IF_LINT ( = 0);
  mode_t dst_mode_bits;
  mode_t omitted_permissions;
  bool restore_dst_mode = false;
  char *earlier_file = NULL;
  char *dst_backup = NULL;
  bool backup_succeeded = false;
  bool delayed_ok;
  bool copied_as_regular = false;
  bool dest_is_symlink = false;
  bool have_dst_lstat = false;

  if (x->move_mode && rename_succeeded)
    *rename_succeeded = false;

  *copy_into_self = false;

  if (XSTAT (x, src_name, &src_sb) != 0)
    {
      error (0, errno, _("cannot stat %s"), quote (src_name));
      return false;
    }

  src_mode = src_sb.st_mode;

  if (S_ISDIR (src_mode) && !x->recursive)
    {
      error (0, 0, _("omitting directory %s"), quote (src_name));
      return false;
    }

  /* Detect the case in which the same source file appears more than
     once on the command line and no backup option has been selected.
     If so, simply warn and don't copy it the second time.
     This check is enabled only if x->src_info is non-NULL.  */
  if (command_line_arg)
    {
      if ( ! S_ISDIR (src_sb.st_mode)
           && x->backup_type == no_backups
           && seen_file (x->src_info, src_name, &src_sb))
        {
          error (0, 0, _("warning: source file %s specified more than once"),
                 quote (src_name));
          return true;
        }

      record_file (x->src_info, src_name, &src_sb);
    }

  if (!new_dst)
    {
      /* Regular files can be created by writing through symbolic
         links, but other files cannot.  So use stat on the
         destination when copying a regular file, and lstat otherwise.
         However, if we intend to unlink or remove the destination
         first, use lstat, since a copy won't actually be made to the
         destination in that case.  */
      bool use_stat =
        ((S_ISREG (src_mode)
          || (x->copy_as_regular
              && ! (S_ISDIR (src_mode) || S_ISLNK (src_mode))))
         && ! (x->move_mode || x->symbolic_link || x->hard_link
               || x->backup_type != no_backups
               || x->unlink_dest_before_opening));
      if ((use_stat
           ? stat (dst_name, &dst_sb)
           : lstat (dst_name, &dst_sb))
          != 0)
        {
          if (errno != ENOENT)
            {
              error (0, errno, _("cannot stat %s"), quote (dst_name));
              return false;
            }
          else
            {
              new_dst = true;
            }
        }
      else
        { /* Here, we know that dst_name exists, at least to the point
             that it is stat'able or lstat'able.  */
          bool return_now;
          bool unlink_src;

          have_dst_lstat = !use_stat;
          if (! same_file_ok (src_name, &src_sb, dst_name, &dst_sb,
                              x, &return_now, &unlink_src))
            {
              error (0, 0, _("%s and %s are the same file"),
                     quote_n (0, src_name), quote_n (1, dst_name));
              return false;
            }

          if (!S_ISDIR (src_mode) && x->update)
            {
              /* When preserving time stamps (but not moving within a file
                 system), don't worry if the destination time stamp is
                 less than the source merely because of time stamp
                 truncation.  */
              int options = ((x->preserve_timestamps
                              && ! (x->move_mode
                                    && dst_sb.st_dev == src_sb.st_dev))
                             ? UTIMECMP_TRUNCATE_SOURCE
                             : 0);

              if (0 <= utimecmp (dst_name, &dst_sb, &src_sb, options))
                {
                  /* We're using --update and the destination is not older
                     than the source, so do not copy or move.  Pretend the
                     rename succeeded, so the caller (if it's mv) doesn't
                     end up removing the source file.  */
                  if (rename_succeeded)
                    *rename_succeeded = true;
                  return true;
                }
            }

          /* When there is an existing destination file, we may end up
             returning early, and hence not copying/moving the file.
             This may be due to an interactive `negative' reply to the
             prompt about the existing file.  It may also be due to the
             use of the --reply=no option.

             cp and mv treat -i and -f differently.  */
          if (x->move_mode)
            {
              if (abandon_move (x, dst_name, &dst_sb)
                  || (unlink_src && unlink (src_name) == 0))
                {
                  /* Pretend the rename succeeded, so the caller (mv)
                     doesn't end up removing the source file.  */
                  if (rename_succeeded)
                    *rename_succeeded = true;
                  if (unlink_src && x->verbose)
                    printf (_("removed %s\n"), quote (src_name));
                  return true;
                }
              if (unlink_src)
                {
                  error (0, errno, _("cannot remove %s"), quote (src_name));
                  return false;
                }
            }
          else
            {
              if (! S_ISDIR (src_mode)
                  && (x->interactive == I_ALWAYS_NO
                      || (x->interactive == I_ASK_USER
                          && (overwrite_prompt (dst_name, &dst_sb), 1)
                          && ! yesno ())))
                return true;
            }

          if (return_now)
            return true;

          if (!S_ISDIR (dst_sb.st_mode))
            {
              if (S_ISDIR (src_mode))
                {
                  if (x->move_mode && x->backup_type != no_backups)
                    {
                      /* Moving a directory onto an existing
                         non-directory is ok only with --backup.  */
                    }
                  else
                    {
                      error (0, 0,
                       _("cannot overwrite non-directory %s with directory %s"),
                             quote_n (0, dst_name), quote_n (1, src_name));
                      return false;
                    }
                }

              /* Don't let the user destroy their data, even if they try hard:
                 This mv command must fail (likewise for cp):
                   rm -rf a b c; mkdir a b c; touch a/f b/f; mv a/f b/f c
                 Otherwise, the contents of b/f would be lost.
                 In the case of `cp', b/f would be lost if the user simulated
                 a move using cp and rm.
                 Note that it works fine if you use --backup=numbered.  */
              if (command_line_arg
                  && x->backup_type != numbered_backups
                  && seen_file (x->dest_info, dst_name, &dst_sb))
                {
                  error (0, 0,
                         _("will not overwrite just-created %s with %s"),
                         quote_n (0, dst_name), quote_n (1, src_name));
                  return false;
                }
            }

          if (!S_ISDIR (src_mode))
            {
              if (S_ISDIR (dst_sb.st_mode))
                {
                  if (x->move_mode && x->backup_type != no_backups)
                    {
                      /* Moving a non-directory onto an existing
                         directory is ok only with --backup.  */
                    }
                  else
                    {
                      error (0, 0,
                         _("cannot overwrite directory %s with non-directory"),
                             quote (dst_name));
                      return false;
                    }
                }
            }

          if (x->move_mode)
            {
              /* Don't allow user to move a directory onto a non-directory.  */
              if (S_ISDIR (src_sb.st_mode) && !S_ISDIR (dst_sb.st_mode)
                  && x->backup_type == no_backups)
                {
                  error (0, 0,
                       _("cannot move directory onto non-directory: %s -> %s"),
                         quote_n (0, src_name), quote_n (0, dst_name));
                  return false;
                }
            }

          if (x->backup_type != no_backups
              /* Don't try to back up a destination if the last
                 component of src_name is "." or "..".  */
              && ! dot_or_dotdot (last_component (src_name))
              /* Create a backup of each destination directory in move mode,
                 but not in copy mode.  FIXME: it might make sense to add an
                 option to suppress backup creation also for move mode.
                 That would let one use mv to merge new content into an
                 existing hierarchy.  */
              && (x->move_mode || ! S_ISDIR (dst_sb.st_mode)))
            {
              char *tmp_backup = find_backup_file_name (dst_name,
                                                        x->backup_type);

              /* Detect (and fail) when creating the backup file would
                 destroy the source file.  Before, running the commands
                 cd /tmp; rm -f a a~; : > a; echo A > a~; cp --b=simple a~ a
                 would leave two zero-length files: a and a~.  */
              /* FIXME: but simply change e.g., the final a~ to `./a~'
                 and the source will still be destroyed.  */
              if (STREQ (tmp_backup, src_name))
                {
                  const char *fmt;
                  fmt = (x->move_mode
                 ? _("backing up %s would destroy source;  %s not moved")
                 : _("backing up %s would destroy source;  %s not copied"));
                  error (0, 0, fmt,
                         quote_n (0, dst_name),
                         quote_n (1, src_name));
                  free (tmp_backup);
                  return false;
                }

              /* FIXME: use fts:
                 Using alloca for a file name that may be arbitrarily
                 long is not recommended.  In fact, even forming such a name
                 should be discouraged.  Eventually, this code will be rewritten
                 to use fts, so using alloca here will be less of a problem.  */
              ASSIGN_STRDUPA (dst_backup, tmp_backup);
              free (tmp_backup);
              if (rename (dst_name, dst_backup) != 0)
                {
                  if (errno != ENOENT)
                    {
                      error (0, errno, _("cannot backup %s"), quote (dst_name));
                      return false;
                    }
                  else
                    {
                      dst_backup = NULL;
                    }
                }
              else
                {
                  backup_succeeded = true;
                }
              new_dst = true;
            }
          else if (! S_ISDIR (dst_sb.st_mode)
                   /* Never unlink dst_name when in move mode.  */
                   && ! x->move_mode
                   && (x->unlink_dest_before_opening
                       || (x->preserve_links && 1 < dst_sb.st_nlink)
                       || (x->dereference == DEREF_NEVER
                           && ! S_ISREG (src_sb.st_mode))
                       ))
            {
              if (unlink (dst_name) != 0 && errno != ENOENT)
                {
                  error (0, errno, _("cannot remove %s"), quote (dst_name));
                  return false;
                }
              new_dst = true;
              if (x->verbose)
                printf (_("removed %s\n"), quote (dst_name));
            }
        }
    }

  /* Ensure we don't try to copy through a symlink that was
     created by a prior call to this function.  */
  if (command_line_arg
      && x->dest_info
      && ! x->move_mode
      && x->backup_type == no_backups)
    {
      bool lstat_ok = true;
      struct stat tmp_buf;
      struct stat *dst_lstat_sb;

      /* If we called lstat above, good: use that data.
         Otherwise, call lstat here, in case dst_name is a symlink.  */
      if (have_dst_lstat)
        dst_lstat_sb = &dst_sb;
      else
        {
          if (lstat (dst_name, &tmp_buf) == 0)
            dst_lstat_sb = &tmp_buf;
          else
            lstat_ok = false;
        }

      /* Never copy through a symlink we've just created.  */
      if (lstat_ok
          && S_ISLNK (dst_lstat_sb->st_mode)
          && seen_file (x->dest_info, dst_name, dst_lstat_sb))
        {
          error (0, 0,
                 _("will not copy %s through just-created symlink %s"),
                 quote_n (0, src_name), quote_n (1, dst_name));
          return false;
        }
    }

  /* If the source is a directory, we don't always create the destination
     directory.  So --verbose should not announce anything until we're
     sure we'll create a directory. */
  if (x->verbose && !S_ISDIR (src_mode))
    emit_verbose (src_name, dst_name, backup_succeeded ? dst_backup : NULL);

  /* Associate the destination file name with the source device and inode
     so that if we encounter a matching dev/ino pair in the source tree
     we can arrange to create a hard link between the corresponding names
     in the destination tree.

     When using the --link (-l) option, there is no need to take special
     measures, because (barring race conditions) files that are hard-linked
     in the source tree will also be hard-linked in the destination tree.

     Sometimes, when preserving links, we have to record dev/ino even
     though st_nlink == 1:
     - when in move_mode, since we may be moving a group of N hard-linked
        files (via two or more command line arguments) to a different
        partition; the links may be distributed among the command line
        arguments (possibly hierarchies) so that the link count of
        the final, once-linked source file is reduced to 1 when it is
        considered below.  But in this case (for mv) we don't need to
        incur the expense of recording the dev/ino => name mapping; all we
        really need is a lookup, to see if the dev/ino pair has already
        been copied.
     - when using -H and processing a command line argument;
        that command line argument could be a symlink pointing to another
        command line argument.  With `cp -H --preserve=link', we hard-link
        those two destination files.
     - likewise for -L except that it applies to all files, not just
        command line arguments.

     Also, with --recursive, record dev/ino of each command-line directory.
     We'll use that info to detect this problem: cp -R dir dir.  */

  if (x->move_mode && src_sb.st_nlink == 1)
    {
      earlier_file = src_to_dest_lookup (src_sb.st_ino, src_sb.st_dev);
    }
  else if (x->preserve_links
           && !x->hard_link
           && (1 < src_sb.st_nlink
               || (command_line_arg
                   && x->dereference == DEREF_COMMAND_LINE_ARGUMENTS)
               || x->dereference == DEREF_ALWAYS))
    {
      earlier_file = remember_copied (dst_name, src_sb.st_ino, src_sb.st_dev);
    }
  else if (x->recursive && S_ISDIR (src_mode))
    {
      if (command_line_arg)
        earlier_file = remember_copied (dst_name, src_sb.st_ino, src_sb.st_dev);
      else
        earlier_file = src_to_dest_lookup (src_sb.st_ino, src_sb.st_dev);
    }

  /* Did we copy this inode somewhere else (in this command line argument)
     and therefore this is a second hard link to the inode?  */

  if (earlier_file)
    {
      /* Avoid damaging the destination file system by refusing to preserve
         hard-linked directories (which are found at least in Netapp snapshot
         directories).  */
      if (S_ISDIR (src_mode))
        {
          /* If src_name and earlier_file refer to the same directory entry,
             then warn about copying a directory into itself.  */
          if (same_name (src_name, earlier_file))
            {
              error (0, 0, _("cannot copy a directory, %s, into itself, %s"),
                     quote_n (0, top_level_src_name),
                     quote_n (1, top_level_dst_name));
              *copy_into_self = true;
              goto un_backup;
            }
          else if (x->dereference == DEREF_ALWAYS)
            {
              /* This happens when e.g., encountering a directory for the
                 second or subsequent time via symlinks when cp is invoked
                 with -R and -L.  E.g.,
                 rm -rf a b c d; mkdir a b c d; ln -s ../c a; ln -s ../c b;
                 cp -RL a b d
              */
            }
          else
            {
              error (0, 0, _("will not create hard link %s to directory %s"),
                     quote_n (0, dst_name), quote_n (1, earlier_file));
              goto un_backup;
            }
        }
      else
        {
          /* We want to guarantee that symlinks are not followed.  */
          bool link_failed = (linkat (AT_FDCWD, earlier_file, AT_FDCWD,
                                      dst_name, 0) != 0);

          /* If the link failed because of an existing destination,
             remove that file and then call link again.  */
          if (link_failed && errno == EEXIST)
            {
              if (unlink (dst_name) != 0)
                {
                  error (0, errno, _("cannot remove %s"), quote (dst_name));
                  goto un_backup;
                }
              if (x->verbose)
                printf (_("removed %s\n"), quote (dst_name));
              link_failed = (linkat (AT_FDCWD, earlier_file, AT_FDCWD,
                                     dst_name, 0) != 0);
            }

          if (link_failed)
            {
              error (0, errno, _("cannot create hard link %s to %s"),
                     quote_n (0, dst_name), quote_n (1, earlier_file));
              goto un_backup;
            }

          return true;
        }
    }

  if (x->move_mode)
    {
      if (rename (src_name, dst_name) == 0)
        {
          if (x->verbose && S_ISDIR (src_mode))
            emit_verbose (src_name, dst_name,
                          backup_succeeded ? dst_backup : NULL);

          if (rename_succeeded)
            *rename_succeeded = true;

          if (command_line_arg)
            {
              /* Record destination dev/ino/name, so that if we are asked
                 to overwrite that file again, we can detect it and fail.  */
              /* It's fine to use the _source_ stat buffer (src_sb) to get the
                 _destination_ dev/ino, since the rename above can't have
                 changed those, and `mv' always uses lstat.
                 We could limit it further by operating
                 only on non-directories.  */
              record_file (x->dest_info, dst_name, &src_sb);
            }

          return true;
        }

      /* FIXME: someday, consider what to do when moving a directory into
         itself but when source and destination are on different devices.  */

      /* This happens when attempting to rename a directory to a
         subdirectory of itself.  */
      if (errno == EINVAL)
        {
          /* FIXME: this is a little fragile in that it relies on rename(2)
             failing with a specific errno value.  Expect problems on
             non-POSIX systems.  */
          error (0, 0, _("cannot move %s to a subdirectory of itself, %s"),
                 quote_n (0, top_level_src_name),
                 quote_n (1, top_level_dst_name));

          /* Note that there is no need to call forget_created here,
             (compare with the other calls in this file) since the
             destination directory didn't exist before.  */

          *copy_into_self = true;
          /* FIXME-cleanup: Don't return true here; adjust mv.c accordingly.
             The only caller that uses this code (mv.c) ends up setting its
             exit status to nonzero when copy_into_self is nonzero.  */
          return true;
        }

      /* WARNING: there probably exist systems for which an inter-device
         rename fails with a value of errno not handled here.
         If/as those are reported, add them to the condition below.
         If this happens to you, please do the following and send the output
         to the bug-reporting address (e.g., in the output of cp --help):
           touch k; perl -e 'rename "k","/tmp/k" or print "$!(",$!+0,")\n"'
         where your current directory is on one partion and /tmp is the other.
         Also, please try to find the E* errno macro name corresponding to
         the diagnostic and parenthesized integer, and include that in your
         e-mail.  One way to do that is to run a command like this
           find /usr/include/. -type f \
             | xargs grep 'define.*\<E[A-Z]*\>.*\<18\>' /dev/null
         where you'd replace `18' with the integer in parentheses that
         was output from the perl one-liner above.
         If necessary, of course, change `/tmp' to some other directory.  */
      if (errno != EXDEV)
        {
          /* There are many ways this can happen due to a race condition.
             When something happens between the initial XSTAT and the
             subsequent rename, we can get many different types of errors.
             For example, if the destination is initially a non-directory
             or non-existent, but it is created as a directory, the rename
             fails.  If two `mv' commands try to rename the same file at
             about the same time, one will succeed and the other will fail.
             If the permissions on the directory containing the source or
             destination file are made too restrictive, the rename will
             fail.  Etc.  */
          error (0, errno,
                 _("cannot move %s to %s"),
                 quote_n (0, src_name), quote_n (1, dst_name));
          forget_created (src_sb.st_ino, src_sb.st_dev);
          return false;
        }

      /* The rename attempt has failed.  Remove any existing destination
         file so that a cross-device `mv' acts as if it were really using
         the rename syscall.  */
      if (unlink (dst_name) != 0 && errno != ENOENT)
        {
          error (0, errno,
             _("inter-device move failed: %s to %s; unable to remove target"),
                 quote_n (0, src_name), quote_n (1, dst_name));
          forget_created (src_sb.st_ino, src_sb.st_dev);
          return false;
        }

      new_dst = true;
    }

  /* If the ownership might change, or if it is a directory (whose
     special mode bits may change after the directory is created),
     omit some permissions at first, so unauthorized users cannot nip
     in before the file is ready.  */
  dst_mode_bits = (x->set_mode ? x->mode : src_mode) & CHMOD_MODE_BITS;
  omitted_permissions =
    (dst_mode_bits
     & (x->preserve_ownership ? S_IRWXG | S_IRWXO
        : S_ISDIR (src_mode) ? S_IWGRP | S_IWOTH
        : 0));

  delayed_ok = true;

  if (x->preserve_security_context)
    {
      bool all_errors = !x->data_copy_required || x->require_preserve_context;
      bool some_errors = !all_errors && !x->reduce_diagnostics;
      security_context_t con;

      if (0 <= lgetfilecon (src_name, &con))
        {
          if (setfscreatecon (con) < 0)
            {
              if (all_errors || (some_errors && !errno_unsupported (errno)))
                error (0, errno,
                       _("failed to set default file creation context to %s"),
                       quote (con));
              if (x->require_preserve_context)
                {
                  freecon (con);
                  return false;
                }
            }
          freecon (con);
        }
      else
        {
          if (all_errors || (some_errors && !errno_unsupported (errno)))
            {
              error (0, errno,
                     _("failed to get security context of %s"),
                     quote (src_name));
            }
          if (x->require_preserve_context)
            return false;
        }
    }

  if (S_ISDIR (src_mode))
    {
      struct dir_list *dir;

      /* If this directory has been copied before during the
         recursion, there is a symbolic link to an ancestor
         directory of the symbolic link.  It is impossible to
         continue to copy this, unless we've got an infinite disk.  */

      if (is_ancestor (&src_sb, ancestors))
        {
          error (0, 0, _("cannot copy cyclic symbolic link %s"),
                 quote (src_name));
          goto un_backup;
        }

      /* Insert the current directory in the list of parents.  */

      dir = alloca (sizeof *dir);
      dir->parent = ancestors;
      dir->ino = src_sb.st_ino;
      dir->dev = src_sb.st_dev;

      if (new_dst || !S_ISDIR (dst_sb.st_mode))
        {
          /* POSIX says mkdir's behavior is implementation-defined when
             (src_mode & ~S_IRWXUGO) != 0.  However, common practice is
             to ask mkdir to copy all the CHMOD_MODE_BITS, letting mkdir
             decide what to do with S_ISUID | S_ISGID | S_ISVTX.  */
          if (mkdir (dst_name, dst_mode_bits & ~omitted_permissions) != 0)
            {
              error (0, errno, _("cannot create directory %s"),
                     quote (dst_name));
              goto un_backup;
            }

          /* We need search and write permissions to the new directory
             for writing the directory's contents. Check if these
             permissions are there.  */

          if (lstat (dst_name, &dst_sb) != 0)
            {
              error (0, errno, _("cannot stat %s"), quote (dst_name));
              goto un_backup;
            }
          else if ((dst_sb.st_mode & S_IRWXU) != S_IRWXU)
            {
              /* Make the new directory searchable and writable.  */

              dst_mode = dst_sb.st_mode;
              restore_dst_mode = true;

              if (lchmod (dst_name, dst_mode | S_IRWXU) != 0)
                {
                  error (0, errno, _("setting permissions for %s"),
                         quote (dst_name));
                  goto un_backup;
                }
            }

          /* Record the created directory's inode and device numbers into
             the search structure, so that we can avoid copying it again.
             Do this only for the first directory that is created for each
             source command line argument.  */
          if (!*first_dir_created_per_command_line_arg)
            {
              remember_copied (dst_name, dst_sb.st_ino, dst_sb.st_dev);
              *first_dir_created_per_command_line_arg = true;
            }

          if (x->verbose)
            emit_verbose (src_name, dst_name, NULL);
        }

      /* Decide whether to copy the contents of the directory.  */
      if (x->one_file_system && device != 0 && device != src_sb.st_dev)
        {
          /* Here, we are crossing a file system boundary and cp's -x option
             is in effect: so don't copy the contents of this directory. */
        }
      else
        {
          /* Copy the contents of the directory.  Don't just return if
             this fails -- otherwise, the failure to read a single file
             in a source directory would cause the containing destination
             directory not to have owner/perms set properly.  */
          delayed_ok = copy_dir (src_name, dst_name, new_dst, &src_sb, dir, x,
                                 first_dir_created_per_command_line_arg,
                                 copy_into_self);
        }
    }
  else if (x->symbolic_link)
    {
      dest_is_symlink = true;
      if (*src_name != '/')
        {
          /* Check that DST_NAME denotes a file in the current directory.  */
          struct stat dot_sb;
          struct stat dst_parent_sb;
          char *dst_parent;
          bool in_current_dir;

          dst_parent = dir_name (dst_name);

          in_current_dir = (STREQ (".", dst_parent)
                            /* If either stat call fails, it's ok not to report
                               the failure and say dst_name is in the current
                               directory.  Other things will fail later.  */
                            || stat (".", &dot_sb) != 0
                            || stat (dst_parent, &dst_parent_sb) != 0
                            || SAME_INODE (dot_sb, dst_parent_sb));
          free (dst_parent);

          if (! in_current_dir)
            {
              error (0, 0,
           _("%s: can make relative symbolic links only in current directory"),
                     quote (dst_name));
              goto un_backup;
            }
        }
      if (symlink (src_name, dst_name) != 0)
        {
          error (0, errno, _("cannot create symbolic link %s to %s"),
                 quote_n (0, dst_name), quote_n (1, src_name));
          goto un_backup;
        }
    }

  /* cp, invoked with `--link --no-dereference', should not follow the
     link; we guarantee this with gnulib's linkat module (on systems
     where link(2) follows the link, gnulib creates a symlink with
     identical contents, which is good enough for our purposes).  */
  else if (x->hard_link
           && (!S_ISLNK (src_mode)
               || x->dereference != DEREF_NEVER))
    {
       if (linkat (AT_FDCWD, src_name, AT_FDCWD, dst_name, 0))
        {
          error (0, errno, _("cannot create link %s"), quote (dst_name));
          goto un_backup;
        }
    }
  else if (S_ISREG (src_mode)
           || (x->copy_as_regular && !S_ISLNK (src_mode)))
    {
      copied_as_regular = true;
      /* POSIX says the permission bits of the source file must be
         used as the 3rd argument in the open call.  Historical
         practice passed all the source mode bits to 'open', but the extra
         bits were ignored, so it should be the same either way.  */
      if (! copy_reg (src_name, dst_name, x, src_mode & S_IRWXUGO,
                      omitted_permissions, &new_dst, &src_sb))
        goto un_backup;
    }
  else if (S_ISFIFO (src_mode))
    {
      /* Use mknod, rather than mkfifo, because the former preserves
         the special mode bits of a fifo on Solaris 10, while mkfifo
         does not.  But fall back on mkfifo, because on some BSD systems,
         mknod always fails when asked to create a FIFO.  */
      if (mknod (dst_name, src_mode & ~omitted_permissions, 0) != 0)
        if (mkfifo (dst_name, src_mode & ~S_IFIFO & ~omitted_permissions) != 0)
          {
            error (0, errno, _("cannot create fifo %s"), quote (dst_name));
            goto un_backup;
          }
    }
  else if (S_ISBLK (src_mode) || S_ISCHR (src_mode) || S_ISSOCK (src_mode))
    {
      if (mknod (dst_name, src_mode & ~omitted_permissions, src_sb.st_rdev)
          != 0)
        {
          error (0, errno, _("cannot create special file %s"),
                 quote (dst_name));
          goto un_backup;
        }
    }
  else if (S_ISLNK (src_mode))
    {
      char *src_link_val = areadlink_with_size (src_name, src_sb.st_size);
      dest_is_symlink = true;
      if (src_link_val == NULL)
        {
          error (0, errno, _("cannot read symbolic link %s"), quote (src_name));
          goto un_backup;
        }

      if (symlink (src_link_val, dst_name) == 0)
        free (src_link_val);
      else
        {
          int saved_errno = errno;
          bool same_link = false;
          if (x->update && !new_dst && S_ISLNK (dst_sb.st_mode)
              && dst_sb.st_size == strlen (src_link_val))
            {
              /* See if the destination is already the desired symlink.
                 FIXME: This behavior isn't documented, and seems wrong
                 in some cases, e.g., if the destination symlink has the
                 wrong ownership, permissions, or time stamps.  */
              char *dest_link_val =
                areadlink_with_size (dst_name, dst_sb.st_size);
              if (dest_link_val && STREQ (dest_link_val, src_link_val))
                same_link = true;
              free (dest_link_val);
            }
          free (src_link_val);

          if (! same_link)
            {
              error (0, saved_errno, _("cannot create symbolic link %s"),
                     quote (dst_name));
              goto un_backup;
            }
        }

      if (x->preserve_security_context)
        restore_default_fscreatecon_or_die ();

      if (x->preserve_ownership)
        {
          /* Preserve the owner and group of the just-`copied'
             symbolic link, if possible.  */
          if (HAVE_LCHOWN
              && lchown (dst_name, src_sb.st_uid, src_sb.st_gid) != 0
              && ! chown_failure_ok (x))
            {
              error (0, errno, _("failed to preserve ownership for %s"),
                     dst_name);
              goto un_backup;
            }
          else
            {
              /* Can't preserve ownership of symlinks.
                 FIXME: maybe give a warning or even error for symlinks
                 in directories with the sticky bit set -- there, not
                 preserving owner/group is a potential security problem.  */
            }
        }
    }
  else
    {
      error (0, 0, _("%s has unknown file type"), quote (src_name));
      goto un_backup;
    }

  if (command_line_arg && x->dest_info)
    {
      /* Now that the destination file is very likely to exist,
         add its info to the set.  */
      struct stat sb;
      if (lstat (dst_name, &sb) == 0)
        record_file (x->dest_info, dst_name, &sb);
    }

  /* If we've just created a hard-link due to cp's --link option,
     we're done.  */
  if (x->hard_link && ! S_ISDIR (src_mode))
    return delayed_ok;

  if (copied_as_regular)
    return delayed_ok;

  /* POSIX says that `cp -p' must restore the following:
     - permission bits
     - setuid, setgid bits
     - owner and group
     If it fails to restore any of those, we may give a warning but
     the destination must not be removed.
     FIXME: implement the above. */

  /* Adjust the times (and if possible, ownership) for the copy.
     chown turns off set[ug]id bits for non-root,
     so do the chmod last.  */

  if (x->preserve_timestamps)
    {
      struct timespec timespec[2];
      timespec[0] = get_stat_atime (&src_sb);
      timespec[1] = get_stat_mtime (&src_sb);

      if ((dest_is_symlink
           ? utimens_symlink (dst_name, timespec)
           : utimens (dst_name, timespec))
          != 0)
        {
          error (0, errno, _("preserving times for %s"), quote (dst_name));
          if (x->require_preserve)
            return false;
        }
    }

  /* The operations beyond this point may dereference a symlink.  */
  if (dest_is_symlink)
    return delayed_ok;

  /* Avoid calling chown if we know it's not necessary.  */
  if (x->preserve_ownership
      && (new_dst || !SAME_OWNER_AND_GROUP (src_sb, dst_sb)))
    {
      switch (set_owner (x, dst_name, -1, &src_sb, new_dst, &dst_sb))
        {
        case -1:
          return false;

        case 0:
          src_mode &= ~ (S_ISUID | S_ISGID | S_ISVTX);
          break;
        }
    }

  set_author (dst_name, -1, &src_sb);

  if (x->preserve_xattr && ! copy_attr (src_name, -1, dst_name, -1, x)
      && x->require_preserve_xattr)
    return false;

  if (x->preserve_mode || x->move_mode)
    {
      if (copy_acl (src_name, -1, dst_name, -1, src_mode) != 0
          && x->require_preserve)
        return false;
    }
  else if (x->set_mode)
    {
      if (set_acl (dst_name, -1, x->mode) != 0)
        return false;
    }
  else
    {
      if (omitted_permissions)
        {
          omitted_permissions &= ~ cached_umask ();

          if (omitted_permissions && !restore_dst_mode)
            {
              /* Permissions were deliberately omitted when the file
                 was created due to security concerns.  See whether
                 they need to be re-added now.  It'd be faster to omit
                 the lstat, but deducing the current destination mode
                 is tricky in the presence of implementation-defined
                 rules for special mode bits.  */
              if (new_dst && lstat (dst_name, &dst_sb) != 0)
                {
                  error (0, errno, _("cannot stat %s"), quote (dst_name));
                  return false;
                }
              dst_mode = dst_sb.st_mode;
              if (omitted_permissions & ~dst_mode)
                restore_dst_mode = true;
            }
        }

      if (restore_dst_mode)
        {
          if (lchmod (dst_name, dst_mode | omitted_permissions) != 0)
            {
              error (0, errno, _("preserving permissions for %s"),
                     quote (dst_name));
              if (x->require_preserve)
                return false;
            }
        }
    }

  return delayed_ok;

un_backup:

  if (x->preserve_security_context)
    restore_default_fscreatecon_or_die ();

  /* We have failed to create the destination file.
     If we've just added a dev/ino entry via the remember_copied
     call above (i.e., unless we've just failed to create a hard link),
     remove the entry associating the source dev/ino with the
     destination file name, so we don't try to `preserve' a link
     to a file we didn't create.  */
  if (earlier_file == NULL)
    forget_created (src_sb.st_ino, src_sb.st_dev);

  if (dst_backup)
    {
      if (rename (dst_backup, dst_name) != 0)
        error (0, errno, _("cannot un-backup %s"), quote (dst_name));
      else
        {
          if (x->verbose)
            printf (_("%s -> %s (unbackup)\n"),
                    quote_n (0, dst_backup), quote_n (1, dst_name));
        }
    }
  return false;
}

