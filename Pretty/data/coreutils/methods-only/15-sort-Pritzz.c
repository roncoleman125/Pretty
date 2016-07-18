/* Create a new temporary file, returning its newly allocated tempnode.
   Store into *PFD the file descriptor open for writing.
   If the creation fails, return NULL and store -1 into *PFD if the
   failure is due to file descriptor exhaustion and
   SURVIVE_FD_EXHAUSTION; otherwise, die.  */

static struct tempnode *
create_temp_file (int *pfd, bool survive_fd_exhaustion)
{
  static char const slashbase[] = "/sortXXXXXX";
  static size_t temp_dir_index;
  int fd;
  int saved_errno;
  char const *temp_dir = temp_dirs[temp_dir_index];
  size_t len = strlen (temp_dir);
  struct tempnode *node =
    xmalloc (offsetof (struct tempnode, name) + len + sizeof slashbase);
  char *file = node->name;
  struct cs_status cs;

  memcpy (file, temp_dir, len);
  memcpy (file + len, slashbase, sizeof slashbase);
  node->next = NULL;
  if (++temp_dir_index == temp_dir_count)
    temp_dir_index = 0;

  /* Create the temporary file in a critical section, to avoid races.  */
  cs = cs_enter ();
  fd = mkstemp (file);
  if (0 <= fd)
    {
      *temptail = node;
      temptail = &node->next;
    }
  saved_errno = errno;
  cs_leave (cs);
  errno = saved_errno;

  if (fd < 0)
    {
      if (! (survive_fd_exhaustion && errno == EMFILE))
        error (SORT_FAILURE, errno, _("cannot create temporary file in %s"),
               quote (temp_dir));
      free (node);
      node = NULL;
    }

  *pfd = fd;
  return node;
}

/* Return a stream for FILE, opened with mode HOW.  A null FILE means
   standard output; HOW should be "w".  When opening for input, "-"
   means standard input.  To avoid confusion, do not return file
   descriptors STDIN_FILENO, STDOUT_FILENO, or STDERR_FILENO when
   opening an ordinary FILE.  Return NULL if unsuccessful.

   fadvise() is used to specify an access pattern for input files.
   There are a few hints we could possibly provide,
   and after careful testing it was decided that
   specifying POSIX_FADV_SEQUENTIAL was not detrimental
   to any cases.  On Linux 2.6.31, this option doubles
   the size of read ahead performed and thus was seen to
   benefit these cases:
     Merging
     Sorting with a smaller internal buffer
     Reading from faster flash devices

   In _addition_ one could also specify other hints...

   POSIX_FADV_WILLNEED was tested, but Linux 2.6.31
   at least uses that to _synchronously_ prepopulate the cache
   with the specified range.  While sort does need to
   read all of its input before outputting, a synchronous
   read of the whole file up front precludes any processing
   that sort could do in parallel with the system doing
   read ahead of the data. This was seen to have negative effects
   in a couple of cases:
     Merging
     Sorting with a smaller internal buffer
   Note this option was seen to shorten the runtime for sort
   on a multicore system with lots of RAM and other processes
   competing for CPU.  It could be argued that more explicit
   scheduling hints with `nice` et. al. are more appropriate
   for this situation.

   POSIX_FADV_NOREUSE is a possibility as it could lower
   the priority of input data in the cache as sort will
   only need to process it once.  However its functionality
   has changed over Linux kernel versions and as of 2.6.31
   it does nothing and thus we can't depend on what it might
   do in future.

   POSIX_FADV_DONTNEED is not appropriate for user specified
   input files, but for temp files we do want to drop the
   cache immediately after processing.  This is done implicitly
   however when the files are unlinked.  */
