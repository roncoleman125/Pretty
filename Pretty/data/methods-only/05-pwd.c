/* Determine the basename of the current directory, where DOT_SB is the
   result of lstat'ing "." and prepend that to the file name in *FILE_NAME.
   Find the directory entry in `..' that matches the dev/i-node of DOT_SB.
   Upon success, update *DOT_SB with stat information of `..', chdir to `..',
   and prepend "/basename" to FILE_NAME.
   Otherwise, exit with a diagnostic.
   PARENT_HEIGHT is the number of levels `..' is above the starting directory.
   The first time this function is called (from the initial directory),
   PARENT_HEIGHT is 1.  This is solely for diagnostics.
   Exit nonzero upon error.  */

static void
find_dir_entry (struct stat *dot_sb, struct file_name *file_name,
                size_t parent_height)
{
  DIR *dirp;
  int fd;
  struct stat parent_sb;
  bool use_lstat;
  bool found;

  dirp = opendir ("..");
  if (dirp == NULL)
    error (EXIT_FAILURE, errno, _("cannot open directory %s"),
           quote (nth_parent (parent_height)));

  fd = dirfd (dirp);
  if ((0 <= fd ? fchdir (fd) : chdir ("..")) < 0)
    error (EXIT_FAILURE, errno, _("failed to chdir to %s"),
           quote (nth_parent (parent_height)));

  if ((0 <= fd ? fstat (fd, &parent_sb) : stat (".", &parent_sb)) < 0)
    error (EXIT_FAILURE, errno, _("failed to stat %s"),
           quote (nth_parent (parent_height)));

  /* If parent and child directory are on different devices, then we
     can't rely on d_ino for useful i-node numbers; use lstat instead.  */
  use_lstat = (parent_sb.st_dev != dot_sb->st_dev);

  found = false;
  while (1)
    {
      struct dirent const *dp;
      struct stat ent_sb;
      ino_t ino;

      errno = 0;
      if ((dp = readdir_ignoring_dot_and_dotdot (dirp)) == NULL)
        {
          if (errno)
            {
              /* Save/restore errno across closedir call.  */
              int e = errno;
              closedir (dirp);
              errno = e;

              /* Arrange to give a diagnostic after exiting this loop.  */
              dirp = NULL;
            }
          break;
        }

      ino = D_INO (dp);

      if (ino == NOT_AN_INODE_NUMBER || use_lstat)
        {
          if (lstat (dp->d_name, &ent_sb) < 0)
            {
              /* Skip any entry we can't stat.  */
              continue;
            }
          ino = ent_sb.st_ino;
        }

      if (ino != dot_sb->st_ino)
        continue;

      /* If we're not crossing a device boundary, then a simple i-node
         match is enough.  */
      if ( ! use_lstat || ent_sb.st_dev == dot_sb->st_dev)
        {
          file_name_prepend (file_name, dp->d_name, _D_EXACT_NAMLEN (dp));
          found = true;
          break;
        }
    }

  if (dirp == NULL || closedir (dirp) != 0)
    {
      /* Note that this diagnostic serves for both readdir
         and closedir failures.  */
      error (EXIT_FAILURE, errno, _("reading directory %s"),
             quote (nth_parent (parent_height)));
    }

  if ( ! found)
    error (EXIT_FAILURE, 0,
           _("couldn't find directory entry in %s with matching i-node"),
             quote (nth_parent (parent_height)));

  *dot_sb = parent_sb;
}

