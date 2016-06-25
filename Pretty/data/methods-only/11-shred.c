/*
 * Repeatedly rename a file with shorter and shorter names,
 * to obliterate all traces of the file name on any system that
 * adds a trailing delimiter to on-disk file names and reuses
 * the same directory slot.  Finally, unlink it.
 * The passed-in filename is modified in place to the new filename.
 * (Which is unlinked if this function succeeds, but is still present if
 * it fails for some reason.)
 *
 * The main loop is written carefully to not get stuck if all possible
 * names of a given length are occupied.  It counts down the length from
 * the original to 0.  While the length is non-zero, it tries to find an
 * unused file name of the given length.  It continues until either the
 * name is available and the rename succeeds, or it runs out of names
 * to try (incname wraps and returns 1).  Finally, it unlinks the file.
 *
 * The unlink is Unix-specific, as ANSI-standard remove has more
 * portability problems with C libraries making it "safe".  rename
 * is ANSI-standard.
 *
 * To force the directory data out, we try to open the directory and
 * invoke fdatasync and/or fsync on it.  This is non-standard, so don't
 * insist that it works: just fall back to a global sync in that case.
 * This is fairly significantly Unix-specific.  Of course, on any
 * file system with synchronous metadata updates, this is unnecessary.
 */
static bool
wipename (char *oldname, char const *qoldname, struct Options const *flags)
{
  char *newname = xstrdup (oldname);
  char *base = last_component (newname);
  size_t len = base_len (base);
  char *dir = dir_name (newname);
  char *qdir = xstrdup (quotearg_colon (dir));
  bool first = true;
  bool ok = true;

  int dir_fd = open (dir, O_RDONLY | O_DIRECTORY | O_NOCTTY | O_NONBLOCK);

  if (flags->verbose)
    error (0, 0, _("%s: removing"), qoldname);

  while (len)
    {
      memset (base, nameset[0], len);
      base[len] = 0;
      do
        {
          struct stat st;
          if (lstat (newname, &st) < 0)
            {
              if (rename (oldname, newname) == 0)
                {
                  if (0 <= dir_fd && dosync (dir_fd, qdir) != 0)
                    ok = false;
                  if (flags->verbose)
                    {
                      /*
                       * People seem to understand this better than talking
                       * about renaming oldname.  newname doesn't need
                       * quoting because we picked it.  oldname needs to
                       * be quoted only the first time.
                       */
                      char const *old = (first ? qoldname : oldname);
                      error (0, 0, _("%s: renamed to %s"), old, newname);
                      first = false;
                    }
                  memcpy (oldname + (base - newname), base, len + 1);
                  break;
                }
              else
                {
                  /* The rename failed: give up on this length.  */
                  break;
                }
            }
          else
            {
              /* newname exists, so increment BASE so we use another */
            }
        }
      while (incname (base, len));
      len--;
    }
  if (unlink (oldname) != 0)
    {
      error (0, errno, _("%s: failed to remove"), qoldname);
      ok = false;
    }
  else if (flags->verbose)
    error (0, 0, _("%s: removed"), qoldname);
  if (0 <= dir_fd)
    {
      if (dosync (dir_fd, qdir) != 0)
        ok = false;
      if (close (dir_fd) != 0)
        {
          error (0, errno, _("%s: failed to close"), qdir);
          ok = false;
        }
    }
  free (newname);
  free (dir);
  free (qdir);
  return ok;
}

