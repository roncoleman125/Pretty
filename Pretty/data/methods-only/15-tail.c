/* Tail N_FILES files forever, or until killed.
   The pertinent information for each file is stored in an entry of F.
   Loop over each of them, doing an fstat to see if they have changed size,
   and an occasional open/fstat to see if any dev/ino pair has changed.
   If none of them have changed size in one iteration, sleep for a
   while and try again.  Continue until the user interrupts us.  */

static void
tail_forever (struct File_spec *f, size_t n_files, double sleep_interval)
{
  /* Use blocking I/O as an optimization, when it's easy.  */
  bool blocking = (pid == 0 && follow_mode == Follow_descriptor
                   && n_files == 1 && ! S_ISREG (f[0].mode));
  size_t last;
  bool writer_is_dead = false;

  last = n_files - 1;

  while (1)
    {
      size_t i;
      bool any_input = false;

      for (i = 0; i < n_files; i++)
        {
          int fd;
          char const *name;
          mode_t mode;
          struct stat stats;
          uintmax_t bytes_read;

          if (f[i].ignore)
            continue;

          if (f[i].fd < 0)
            {
              recheck (&f[i], blocking);
              continue;
            }

          fd = f[i].fd;
          name = pretty_name (&f[i]);
          mode = f[i].mode;

          if (f[i].blocking != blocking)
            {
              int old_flags = fcntl (fd, F_GETFL);
              int new_flags = old_flags | (blocking ? 0 : O_NONBLOCK);
              if (old_flags < 0
                  || (new_flags != old_flags
                      && fcntl (fd, F_SETFL, new_flags) == -1))
                {
                  /* Don't update f[i].blocking if fcntl fails.  */
                  if (S_ISREG (f[i].mode) && errno == EPERM)
                    {
                      /* This happens when using tail -f on a file with
                         the append-only attribute.  */
                    }
                  else
                    error (EXIT_FAILURE, errno,
                           _("%s: cannot change nonblocking mode"), name);
                }
              else
                f[i].blocking = blocking;
            }

          if (!f[i].blocking)
            {
              if (fstat (fd, &stats) != 0)
                {
                  f[i].fd = -1;
                  f[i].errnum = errno;
                  error (0, errno, "%s", name);
                  continue;
                }

              if (f[i].mode == stats.st_mode
                  && (! S_ISREG (stats.st_mode) || f[i].size == stats.st_size)
                  && timespec_cmp (f[i].mtime, get_stat_mtime (&stats)) == 0)
                {
                  if ((max_n_unchanged_stats_between_opens
                       <= f[i].n_unchanged_stats++)
                      && follow_mode == Follow_name)
                    {
                      recheck (&f[i], f[i].blocking);
                      f[i].n_unchanged_stats = 0;
                    }
                  continue;
                }

              /* This file has changed.  Print out what we can, and
                 then keep looping.  */

              f[i].mtime = get_stat_mtime (&stats);
              f[i].mode = stats.st_mode;

              /* reset counter */
              f[i].n_unchanged_stats = 0;

              if (S_ISREG (mode) && stats.st_size < f[i].size)
                {
                  error (0, 0, _("%s: file truncated"), name);
                  last = i;
                  xlseek (fd, stats.st_size, SEEK_SET, name);
                  f[i].size = stats.st_size;
                  continue;
                }

              if (i != last)
                {
                  if (print_headers)
                    write_header (name);
                  last = i;
                }
            }

          bytes_read = dump_remainder (name, fd,
                                       (f[i].blocking
                                        ? COPY_A_BUFFER : COPY_TO_EOF));
          any_input |= (bytes_read != 0);
          f[i].size += bytes_read;
        }

      if (! any_live_files (f, n_files) && ! reopen_inaccessible_files)
        {
          error (0, 0, _("no files remaining"));
          break;
        }

      if ((!any_input || blocking) && fflush (stdout) != 0)
        error (EXIT_FAILURE, errno, _("write error"));

      /* If nothing was read, sleep and/or check for dead writers.  */
      if (!any_input)
        {
          if (writer_is_dead)
            break;

          /* Once the writer is dead, read the files once more to
             avoid a race condition.  */
          writer_is_dead = (pid != 0
                            && kill (pid, 0) != 0
                            /* Handle the case in which you cannot send a
                               signal to the writer, so kill fails and sets
                               errno to EPERM.  */
                            && errno != EPERM);

          if (!writer_is_dead && xnanosleep (sleep_interval))
            error (EXIT_FAILURE, errno, _("cannot read realtime clock"));

        }
    }
}


/* Return true if any of the N_FILES files in F is remote, i.e., has
   an open file descriptor and is on a network file system.  */

static bool
any_remote_file (const struct File_spec *f, size_t n_files)
{
  size_t i;

  for (i = 0; i < n_files; i++)
    if (0 <= f[i].fd && f[i].remote)
      return true;
  return false;
}

/* Return true if any of the N_FILES files in F represents
   stdin and is tailable.  */

static bool
tailable_stdin (const struct File_spec *f, size_t n_files)
{
  size_t i;

  for (i = 0; i < n_files; i++)
    if (!f[i].ignore && STREQ (f[i].name, "-"))
      return true;
  return false;
}

static size_t
wd_hasher (const void *entry, size_t tabsize)
{
  const struct File_spec *spec = entry;
  return spec->wd % tabsize;
}

static bool
wd_comparator (const void *e1, const void *e2)
{
  const struct File_spec *spec1 = e1;
  const struct File_spec *spec2 = e2;
  return spec1->wd == spec2->wd;
}

/* Helper function used by `tail_forever_inotify'.  */
static void
check_fspec (struct File_spec *fspec, int wd, int *prev_wd)
{
  struct stat stats;
  char const *name = pretty_name (fspec);

  if (fstat (fspec->fd, &stats) != 0)
    {
      close_fd (fspec->fd, name);
      fspec->fd = -1;
      fspec->errnum = errno;
      return;
    }

  if (S_ISREG (fspec->mode) && stats.st_size < fspec->size)
    {
      error (0, 0, _("%s: file truncated"), name);
      *prev_wd = wd;
      xlseek (fspec->fd, stats.st_size, SEEK_SET, name);
      fspec->size = stats.st_size;
    }
  else if (S_ISREG (fspec->mode) && stats.st_size == fspec->size
           && timespec_cmp (fspec->mtime, get_stat_mtime (&stats)) == 0)
    return;

  if (wd != *prev_wd)
    {
      if (print_headers)
        write_header (name);
      *prev_wd = wd;
    }

  uintmax_t bytes_read = dump_remainder (name, fspec->fd, COPY_TO_EOF);
  fspec->size += bytes_read;

  if (fflush (stdout) != 0)
    error (EXIT_FAILURE, errno, _("write error"));
}

/* Attempt to tail N_FILES files forever, or until killed.
   Check modifications using the inotify events system.
   Return false on error, or true to revert to polling.  */
static bool
tail_forever_inotify (int wd, struct File_spec *f, size_t n_files,
                      double sleep_interval)
{
  unsigned int max_realloc = 3;

  /* Map an inotify watch descriptor to the name of the file it's watching.  */
  Hash_table *wd_to_name;

  bool found_watchable_file = false;
  bool found_unwatchable_dir = false;
  bool no_inotify_resources = false;
  bool writer_is_dead = false;
  int prev_wd;
  size_t evlen = 0;
  char *evbuf;
  size_t evbuf_off = 0;
  size_t len = 0;

  wd_to_name = hash_initialize (n_files, NULL, wd_hasher, wd_comparator, NULL);
  if (! wd_to_name)
    xalloc_die ();

  /* Add an inotify watch for each watched file.  If -F is specified then watch
     its parent directory too, in this way when they re-appear we can add them
     again to the watch list.  */
  size_t i;
  for (i = 0; i < n_files; i++)
    {
      if (!f[i].ignore)
        {
          size_t fnlen = strlen (f[i].name);
          if (evlen < fnlen)
            evlen = fnlen;

          f[i].wd = -1;

          if (follow_mode == Follow_name)
            {
              size_t dirlen = dir_len (f[i].name);
              char prev = f[i].name[dirlen];
              f[i].basename_start = last_component (f[i].name) - f[i].name;

              f[i].name[dirlen] = '\0';

               /* It's fine to add the same directory more than once.
                  In that case the same watch descriptor is returned.  */
              f[i].parent_wd = inotify_add_watch (wd, dirlen ? f[i].name : ".",
                                                  (IN_CREATE | IN_MOVED_TO
                                                   | IN_ATTRIB));

              f[i].name[dirlen] = prev;

              if (f[i].parent_wd < 0)
                {
                  if (errno != ENOSPC) /* suppress confusing error.  */
                    error (0, errno, _("cannot watch parent directory of %s"),
                           quote (f[i].name));
                  else
                    error (0, 0, _("inotify resources exhausted"));
                  found_unwatchable_dir = true;
                  /* We revert to polling below.  Note invalid uses
                     of the inotify API will still be diagnosed.  */
                  break;
                }
            }

          f[i].wd = inotify_add_watch (wd, f[i].name, inotify_wd_mask);

          if (f[i].wd < 0)
            {
              if (errno == ENOSPC)
                {
                  no_inotify_resources = true;
                  error (0, 0, _("inotify resources exhausted"));
                }
              else if (errno != f[i].errnum)
                error (0, errno, _("cannot watch %s"), quote (f[i].name));
              continue;
            }

          if (hash_insert (wd_to_name, &(f[i])) == NULL)
            xalloc_die ();

          found_watchable_file = true;
        }
    }

  /* Linux kernel 2.6.24 at least has a bug where eventually, ENOSPC is always
     returned by inotify_add_watch.  In any case we should revert to polling
     when there are no inotify resources.  Also a specified directory may not
     be currently present or accessible, so revert to polling.  */
  if (no_inotify_resources || found_unwatchable_dir)
    {
      /* FIXME: release hash and inotify resources allocated above.  */
      errno = 0;
      return true;
    }
  if (follow_mode == Follow_descriptor && !found_watchable_file)
    return false;

  prev_wd = f[n_files - 1].wd;

  /* Check files again.  New data can be available since last time we checked
     and before they are watched by inotify.  */
  for (i = 0; i < n_files; i++)
    {
      if (!f[i].ignore)
        check_fspec (&f[i], f[i].wd, &prev_wd);
    }

  evlen += sizeof (struct inotify_event) + 1;
  evbuf = xmalloc (evlen);

  /* Wait for inotify events and handle them.  Events on directories make sure
     that watched files can be re-added when -F is used.
     This loop sleeps on the `safe_read' call until a new event is notified.  */
  while (1)
    {
      struct File_spec *fspec;
      struct inotify_event *ev;

      /* When watching a PID, ensure that a read from WD will not block
         indefinitely.  */
      if (pid)
        {
          if (writer_is_dead)
            exit (EXIT_SUCCESS);

          writer_is_dead = (kill (pid, 0) != 0 && errno != EPERM);

          struct timeval delay; /* how long to wait for file changes.  */
          if (writer_is_dead)
            delay.tv_sec = delay.tv_usec = 0;
          else
            {
              delay.tv_sec = (time_t) sleep_interval;
              delay.tv_usec = 1000000 * (sleep_interval - delay.tv_sec);
            }

           fd_set rfd;
           FD_ZERO (&rfd);
           FD_SET (wd, &rfd);

           int file_change = select (wd + 1, &rfd, NULL, NULL, &delay);

           if (file_change == 0)
             continue;
           else if (file_change == -1)
             error (EXIT_FAILURE, errno, _("error monitoring inotify event"));
        }

      if (len <= evbuf_off)
        {
          len = safe_read (wd, evbuf, evlen);
          evbuf_off = 0;

          /* For kernels prior to 2.6.21, read returns 0 when the buffer
             is too small.  */
          if ((len == 0 || (len == SAFE_READ_ERROR && errno == EINVAL))
              && max_realloc--)
            {
              len = 0;
              evlen *= 2;
              evbuf = xrealloc (evbuf, evlen);
              continue;
            }

          if (len == 0 || len == SAFE_READ_ERROR)
            error (EXIT_FAILURE, errno, _("error reading inotify event"));
        }

      ev = (struct inotify_event *) (evbuf + evbuf_off);
      evbuf_off += sizeof (*ev) + ev->len;

      if (ev->len) /* event on ev->name in watched directory  */
        {
          size_t j;
          for (j = 0; j < n_files; j++)
            {
              /* With N=hundreds of frequently-changing files, this O(N^2)
                 process might be a problem.  FIXME: use a hash table?  */
              if (f[j].parent_wd == ev->wd
                  && STREQ (ev->name, f[j].name + f[j].basename_start))
                break;
            }

          /* It is not a watched file.  */
          if (j == n_files)
            continue;

          /* It's fine to add the same file more than once.  */
          int new_wd = inotify_add_watch (wd, f[j].name, inotify_wd_mask);
          if (new_wd < 0)
            {
              error (0, errno, _("cannot watch %s"), quote (f[j].name));
              continue;
            }

          fspec = &(f[j]);

          /* Remove `fspec' and re-add it using `new_fd' as its key.  */
          hash_delete (wd_to_name, fspec);
          fspec->wd = new_wd;

          /* If the file was moved then inotify will use the source file wd for
             the destination file.  Make sure the key is not present in the
             table.  */
          struct File_spec *prev = hash_delete (wd_to_name, fspec);
          if (prev && prev != fspec)
            {
              if (follow_mode == Follow_name)
                recheck (prev, false);
              prev->wd = -1;
              close_fd (prev->fd, pretty_name (prev));
            }

          if (hash_insert (wd_to_name, fspec) == NULL)
            xalloc_die ();

          if (follow_mode == Follow_name)
            recheck (fspec, false);
        }
      else
        {
          struct File_spec key;
          key.wd = ev->wd;
          fspec = hash_lookup (wd_to_name, &key);
        }

      if (! fspec)
        continue;

      if (ev->mask & (IN_ATTRIB | IN_DELETE_SELF | IN_MOVE_SELF))
        {
          /* For IN_DELETE_SELF, we always want to remove the watch.
             However, for IN_MOVE_SELF (the file we're watching has
             been clobbered via a rename), when tailing by NAME, we
             must continue to watch the file.  It's only when following
             by file descriptor that we must remove the watch.  */
          if ((ev->mask & IN_DELETE_SELF)
              || ((ev->mask & IN_MOVE_SELF)
                  && follow_mode == Follow_descriptor))
            {
              inotify_rm_watch (wd, fspec->wd);
              hash_delete (wd_to_name, fspec);
            }
          if (follow_mode == Follow_name)
            recheck (fspec, false);

          continue;
        }
      check_fspec (fspec, ev->wd, &prev_wd);
    }
}

