/* Display the last N_UNITS units of file FILENAME, open for reading
   via FD.  Set *READ_POS to the position of the input stream pointer.
   *READ_POS is usually the number of bytes read and corresponds to an
   offset from the beginning of a file.  However, it may be larger than
   OFF_T_MAX (as for an input pipe), and may also be larger than the
   number of bytes read (when an input pointer is initially not at
   beginning of file), and may be far greater than the number of bytes
   actually read for an input file that is seekable.
   Return true if successful.  */

static bool
tail (const char *filename, int fd, uintmax_t n_units,
      uintmax_t *read_pos)
{
  *read_pos = 0;
  if (count_lines)
    return tail_lines (filename, fd, n_units, read_pos);
  else
    return tail_bytes (filename, fd, n_units, read_pos);
}

/* Display the last N_UNITS units of the file described by F.
   Return true if successful.  */

static bool
tail_file (struct File_spec *f, uintmax_t n_units)
{
  int fd;
  bool ok;

  bool is_stdin = (STREQ (f->name, "-"));

  if (is_stdin)
    {
      have_read_stdin = true;
      fd = STDIN_FILENO;
      if (O_BINARY && ! isatty (STDIN_FILENO))
        xfreopen (NULL, "rb", stdin);
    }
  else
    fd = open (f->name, O_RDONLY | O_BINARY);

  f->tailable = !(reopen_inaccessible_files && fd == -1);

  if (fd == -1)
    {
      if (forever)
        {
          f->fd = -1;
          f->errnum = errno;
          f->ignore = false;
          f->ino = 0;
          f->dev = 0;
        }
      error (0, errno, _("cannot open %s for reading"),
             quote (pretty_name (f)));
      ok = false;
    }
  else
    {
      uintmax_t read_pos;

      if (print_headers)
        write_header (pretty_name (f));
      ok = tail (pretty_name (f), fd, n_units, &read_pos);
      if (forever)
        {
          struct stat stats;

          /* Before the tail function provided `read_pos', there was
             a race condition described in the URL below.  This sleep
             call made the window big enough to exercise the problem.  */
          xnanosleep (1);
          f->errnum = ok - 1;
          if (fstat (fd, &stats) < 0)
            {
              ok = false;
              f->errnum = errno;
              error (0, errno, _("error reading %s"), quote (pretty_name (f)));
            }
          else if (!IS_TAILABLE_FILE_TYPE (stats.st_mode))
            {
              error (0, 0, _("%s: cannot follow end of this type of file;\
 giving up on this name"),
                     pretty_name (f));
              ok = false;
              f->errnum = -1;
              f->ignore = true;
            }

          if (!ok)
            {
              close_fd (fd, pretty_name (f));
              f->fd = -1;
            }
          else
            {
              /* Note: we must use read_pos here, not stats.st_size,
                 to avoid a race condition described by Ken Raeburn:
        http://mail.gnu.org/archive/html/bug-textutils/2003-05/msg00007.html */
              record_open_fd (f, fd, read_pos, &stats, (is_stdin ? -1 : 1));
              f->remote = fremote (fd, pretty_name (f));
            }
        }
      else
        {
          if (!is_stdin && close (fd))
            {
              error (0, errno, _("error reading %s"), quote (pretty_name (f)));
              ok = false;
            }
        }
    }

  return ok;
}

