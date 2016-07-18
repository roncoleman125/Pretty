/* Record a file F with descriptor FD, size SIZE, status ST, and
   blocking status BLOCKING.  */

static void
record_open_fd (struct File_spec *f, int fd,
                off_t size, struct stat const *st,
                int blocking)
{
  f->fd = fd;
  f->size = size;
  f->mtime = get_stat_mtime (st);
  f->dev = st->st_dev;
  f->ino = st->st_ino;
  f->mode = st->st_mode;
  f->blocking = blocking;
  f->n_unchanged_stats = 0;
  f->ignore = false;
}

