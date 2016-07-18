/* Return 1 if FILE is an unwritable non-symlink,
   0 if it is writable or some other type of file,
   -1 and set errno if there is some problem in determining the answer.
   Use FULL_NAME only if necessary.
   Set *BUF to the file status.
   This is to avoid calling euidaccess when FILE is a symlink.  */
static int
write_protected_non_symlink (int fd_cwd,
                             char const *file,
                             char const *full_name,
                             struct stat *buf)
{
  if (can_write_any_file ())
    return 0;
  if (cache_fstatat (fd_cwd, file, buf, AT_SYMLINK_NOFOLLOW) != 0)
    return -1;
  if (S_ISLNK (buf->st_mode))
    return 0;
  /* Here, we know FILE is not a symbolic link.  */

  /* In order to be reentrant -- i.e., to avoid changing the working
     directory, and at the same time to be able to deal with alternate
     access control mechanisms (ACLs, xattr-style attributes) and
     arbitrarily deep trees -- we need a function like eaccessat, i.e.,
     like Solaris' eaccess, but fd-relative, in the spirit of openat.  */

  /* In the absence of a native eaccessat function, here are some of
     the implementation choices [#4 and #5 were suggested by Paul Eggert]:
     1) call openat with O_WRONLY|O_NOCTTY
        Disadvantage: may create the file and doesn't work for directory,
        may mistakenly report `unwritable' for EROFS or ACLs even though
        perm bits say the file is writable.

     2) fake eaccessat (save_cwd, fchdir, call euidaccess, restore_cwd)
        Disadvantage: changes working directory (not reentrant) and can't
        work if save_cwd fails.

     3) if (euidaccess (full_name, W_OK) == 0)
        Disadvantage: doesn't work if full_name is too long.
        Inefficient for very deep trees (O(Depth^2)).

     4) If the full pathname is sufficiently short (say, less than
        PATH_MAX or 8192 bytes, whichever is shorter):
        use method (3) (i.e., euidaccess (full_name, W_OK));
        Otherwise: vfork, fchdir in the child, run euidaccess in the
        child, then the child exits with a status that tells the parent
        whether euidaccess succeeded.

        This avoids the O(N**2) algorithm of method (3), and it also avoids
        the failure-due-to-too-long-file-names of method (3), but it's fast
        in the normal shallow case.  It also avoids the lack-of-reentrancy
        and the save_cwd problems.
        Disadvantage; it uses a process slot for very-long file names,
        and would be very slow for hierarchies with many such files.

     5) If the full file name is sufficiently short (say, less than
        PATH_MAX or 8192 bytes, whichever is shorter):
        use method (3) (i.e., euidaccess (full_name, W_OK));
        Otherwise: look just at the file bits.  Perhaps issue a warning
        the first time this occurs.

        This is like (4), except for the "Otherwise" case where it isn't as
        "perfect" as (4) but is considerably faster.  It conforms to current
        POSIX, and is uniformly better than what Solaris and FreeBSD do (they
        mess up with long file names). */

  {
    /* This implements #1: on decent systems, either faccessat is
       native or /proc/self/fd allows us to skip a chdir.  */
    if (!openat_needs_fchdir ())
      {
        if (faccessat (fd_cwd, file, W_OK, AT_EACCESS) == 0)
          return 0;

        return errno == EACCES ? 1 : -1;
      }

    /* This implements #5: */
    size_t file_name_len = strlen (full_name);

    if (MIN (PATH_MAX, 8192) <= file_name_len)
      return ! euidaccess_stat (buf, W_OK);
    if (euidaccess (full_name, W_OK) == 0)
      return 0;
    if (errno == EACCES)
      {
        errno = 0;
        return 1;
      }

    /* Perhaps some other process has removed the file, or perhaps this
       is a buggy NFS client.  */
    return -1;
  }
}

