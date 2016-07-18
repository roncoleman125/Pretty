/* FIXME: describe */

static void
recheck (struct File_spec *f, bool blocking)
{
  /* open/fstat the file and announce if dev/ino have changed */
  struct stat new_stats;
  bool ok = true;
  bool is_stdin = (STREQ (f->name, "-"));
  bool was_tailable = f->tailable;
  int prev_errnum = f->errnum;
  bool new_file;
  int fd = (is_stdin
            ? STDIN_FILENO
            : open (f->name, O_RDONLY | (blocking ? 0 : O_NONBLOCK)));

  assert (valid_file_spec (f));

  /* If the open fails because the file doesn't exist,
     then mark the file as not tailable.  */
  f->tailable = !(reopen_inaccessible_files && fd == -1);

  if (fd == -1 || fstat (fd, &new_stats) < 0)
    {
      ok = false;
      f->errnum = errno;
      if (!f->tailable)
        {
          if (was_tailable)
            {
              /* FIXME-maybe: detect the case in which the file first becomes
                 unreadable (perms), and later becomes readable again and can
                 be seen to be the same file (dev/ino).  Otherwise, tail prints
                 the entire contents of the file when it becomes readable.  */
              error (0, f->errnum, _("%s has become inaccessible"),
                     quote (pretty_name (f)));
            }
          else
            {
              /* say nothing... it's still not tailable */
            }
        }
      else if (prev_errnum != errno)
        {
          error (0, errno, "%s", pretty_name (f));
        }
    }
  else if (!IS_TAILABLE_FILE_TYPE (new_stats.st_mode))
    {
      ok = false;
      f->errnum = -1;
      error (0, 0, _("%s has been replaced with an untailable file;\
 giving up on this name"),
             quote (pretty_name (f)));
      f->ignore = true;
    }
  else if (!disable_inotify && fremote (fd, pretty_name (f)))
    {
      ok = false;
      f->errnum = -1;
      error (0, 0, _("%s has been replaced with a remote file. "
                     "giving up on this name"), quote (pretty_name (f)));
      f->ignore = true;
      f->remote = true;
    }
  else
    {
      f->errnum = 0;
    }

  new_file = false;
  if (!ok)
    {
      close_fd (fd, pretty_name (f));
      close_fd (f->fd, pretty_name (f));
      f->fd = -1;
    }
  else if (prev_errnum && prev_errnum != ENOENT)
    {
      new_file = true;
      assert (f->fd == -1);
      error (0, 0, _("%s has become accessible"), quote (pretty_name (f)));
    }
  else if (f->ino != new_stats.st_ino || f->dev != new_stats.st_dev)
    {
      new_file = true;
      if (f->fd == -1)
        {
          error (0, 0,
                 _("%s has appeared;  following end of new file"),
                 quote (pretty_name (f)));
        }
      else
        {
          /* Close the old one.  */
          close_fd (f->fd, pretty_name (f));

          /* File has been replaced (e.g., via log rotation) --
             tail the new one.  */
          error (0, 0,
                 _("%s has been replaced;  following end of new file"),
                 quote (pretty_name (f)));
        }
    }
  else
    {
      if (f->fd == -1)
        {
          /* This happens when one iteration finds the file missing,
             then the preceding <dev,inode> pair is reused as the
             file is recreated.  */
          new_file = true;
        }
      else
        {
          close_fd (fd, pretty_name (f));
        }
    }

  if (new_file)
    {
      /* Start at the beginning of the file.  */
      record_open_fd (f, fd, 0, &new_stats, (is_stdin ? -1 : blocking));
      xlseek (fd, 0, SEEK_SET, pretty_name (f));
    }
}

