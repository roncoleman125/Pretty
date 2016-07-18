/* Return true if it's ok that the source and destination
   files are the `same' by some measure.  The goal is to avoid
   making the `copy' operation remove both copies of the file
   in that case, while still allowing the user to e.g., move or
   copy a regular file onto a symlink that points to it.
   Try to minimize the cost of this function in the common case.
   Set *RETURN_NOW if we've determined that the caller has no more
   work to do and should return successfully, right away.

   Set *UNLINK_SRC if we've determined that the caller wants to do
   `rename (a, b)' where `a' and `b' are distinct hard links to the same
   file. In that case, the caller should try to unlink `a' and then return
   successfully.  Ideally, we wouldn't have to do that, and we'd be
   able to rely on rename to remove the source file.  However, POSIX
   mistakenly requires that such a rename call do *nothing* and return
   successfully.  */

static bool
same_file_ok (char const *src_name, struct stat const *src_sb,
              char const *dst_name, struct stat const *dst_sb,
              const struct cp_options *x, bool *return_now, bool *unlink_src)
{
  const struct stat *src_sb_link;
  const struct stat *dst_sb_link;
  struct stat tmp_dst_sb;
  struct stat tmp_src_sb;

  bool same_link;
  bool same = SAME_INODE (*src_sb, *dst_sb);

  *return_now = false;
  *unlink_src = false;

  /* FIXME: this should (at the very least) be moved into the following
     if-block.  More likely, it should be removed, because it inhibits
     making backups.  But removing it will result in a change in behavior
     that will probably have to be documented -- and tests will have to
     be updated.  */
  if (same && x->hard_link)
    {
      *return_now = true;
      return true;
    }

  if (x->dereference == DEREF_NEVER)
    {
      same_link = same;

      /* If both the source and destination files are symlinks (and we'll
         know this here IFF preserving symlinks), then it's ok -- as long
         as they are distinct.  */
      if (S_ISLNK (src_sb->st_mode) && S_ISLNK (dst_sb->st_mode))
        return ! same_name (src_name, dst_name);

      src_sb_link = src_sb;
      dst_sb_link = dst_sb;
    }
  else
    {
      if (!same)
        return true;

      if (lstat (dst_name, &tmp_dst_sb) != 0
          || lstat (src_name, &tmp_src_sb) != 0)
        return true;

      src_sb_link = &tmp_src_sb;
      dst_sb_link = &tmp_dst_sb;

      same_link = SAME_INODE (*src_sb_link, *dst_sb_link);

      /* If both are symlinks, then it's ok, but only if the destination
         will be unlinked before being opened.  This is like the test
         above, but with the addition of the unlink_dest_before_opening
         conjunct because otherwise, with two symlinks to the same target,
         we'd end up truncating the source file.  */
      if (S_ISLNK (src_sb_link->st_mode) && S_ISLNK (dst_sb_link->st_mode)
          && x->unlink_dest_before_opening)
        return true;
    }

  /* The backup code ensures there's a copy, so it's usually ok to
     remove any destination file.  One exception is when both
     source and destination are the same directory entry.  In that
     case, moving the destination file aside (in making the backup)
     would also rename the source file and result in an error.  */
  if (x->backup_type != no_backups)
    {
      if (!same_link)
        {
          /* In copy mode when dereferencing symlinks, if the source is a
             symlink and the dest is not, then backing up the destination
             (moving it aside) would make it a dangling symlink, and the
             subsequent attempt to open it in copy_reg would fail with
             a misleading diagnostic.  Avoid that by returning zero in
             that case so the caller can make cp (or mv when it has to
             resort to reading the source file) fail now.  */

          /* FIXME-note: even with the following kludge, we can still provoke
             the offending diagnostic.  It's just a little harder to do :-)
             $ rm -f a b c; touch c; ln -s c b; ln -s b a; cp -b a b
             cp: cannot open `a' for reading: No such file or directory
             That's misleading, since a subsequent `ls' shows that `a'
             is still there.
             One solution would be to open the source file *before* moving
             aside the destination, but that'd involve a big rewrite. */
          if ( ! x->move_mode
               && x->dereference != DEREF_NEVER
               && S_ISLNK (src_sb_link->st_mode)
               && ! S_ISLNK (dst_sb_link->st_mode))
            return false;

          return true;
        }

      return ! same_name (src_name, dst_name);
    }

  /* FIXME: use or remove */

  /* If we're making a backup, we'll detect the problem case in
     copy_reg because SRC_NAME will no longer exist.  Allowing
     the test to be deferred lets cp do some useful things.
     But when creating hardlinks and SRC_NAME is a symlink
     but DST_NAME is not we must test anyway.  */
  if (x->hard_link
      || !S_ISLNK (src_sb_link->st_mode)
      || S_ISLNK (dst_sb_link->st_mode))
    return true;

  if (x->dereference != DEREF_NEVER)
    return true;

  /* They may refer to the same file if we're in move mode and the
     target is a symlink.  That is ok, since we remove any existing
     destination file before opening it -- via `rename' if they're on
     the same file system, via `unlink (DST_NAME)' otherwise.
     It's also ok if they're distinct hard links to the same file.  */
  if (x->move_mode || x->unlink_dest_before_opening)
    {
      if (S_ISLNK (dst_sb_link->st_mode))
        return true;

      if (same_link
          && 1 < dst_sb_link->st_nlink
          && ! same_name (src_name, dst_name))
        {
          if (x->move_mode)
            {
              *unlink_src = true;
              *return_now = true;
            }
          return true;
        }
    }

  /* If neither is a symlink, then it's ok as long as they aren't
     hard links to the same file.  */
  if (!S_ISLNK (src_sb_link->st_mode) && !S_ISLNK (dst_sb_link->st_mode))
    {
      if (!SAME_INODE (*src_sb_link, *dst_sb_link))
        return true;

      /* If they are the same file, it's ok if we're making hard links.  */
      if (x->hard_link)
        {
          *return_now = true;
          return true;
        }
    }

  /* It's ok to remove a destination symlink.  But that works only when we
     unlink before opening the destination and when the source and destination
     files are on the same partition.  */
  if (x->unlink_dest_before_opening
      && S_ISLNK (dst_sb_link->st_mode))
    return dst_sb_link->st_dev == src_sb_link->st_dev;

  if (x->dereference == DEREF_NEVER)
    {
      if ( ! S_ISLNK (src_sb_link->st_mode))
        tmp_src_sb = *src_sb_link;
      else if (stat (src_name, &tmp_src_sb) != 0)
        return true;

      if ( ! S_ISLNK (dst_sb_link->st_mode))
        tmp_dst_sb = *dst_sb_link;
      else if (stat (dst_name, &tmp_dst_sb) != 0)
        return true;

      if ( ! SAME_INODE (tmp_src_sb, tmp_dst_sb))
        return true;

      /* FIXME: shouldn't this be testing whether we're making symlinks?  */
      if (x->hard_link)
        {
          *return_now = true;
          return true;
        }
    }

  return false;
}

