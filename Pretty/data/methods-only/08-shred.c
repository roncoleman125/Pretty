/*
 * The core routine to actually do the work.  This overwrites the first
 * size bytes of the given fd.  Return true if successful.
 */
static bool
do_wipefd (int fd, char const *qname, struct randint_source *s,
           struct Options const *flags)
{
  size_t i;
  struct stat st;
  off_t size;			/* Size to write, size to read */
  unsigned long int n;		/* Number of passes for printing purposes */
  int *passarray;
  bool ok = true;
  struct randread_source *rs;

  n = 0;		/* dopass takes n -- 0 to mean "don't print progress" */
  if (flags->verbose)
    n = flags->n_iterations + flags->zero_fill;

  if (fstat (fd, &st))
    {
      error (0, errno, _("%s: fstat failed"), qname);
      return false;
    }

  /* If we know that we can't possibly shred the file, give up now.
     Otherwise, we may go into a infinite loop writing data before we
     find that we can't rewind the device.  */
  if ((S_ISCHR (st.st_mode) && isatty (fd))
      || S_ISFIFO (st.st_mode)
      || S_ISSOCK (st.st_mode))
    {
      error (0, 0, _("%s: invalid file type"), qname);
      return false;
    }

  direct_mode (fd, true);

  /* Allocate pass array */
  passarray = xnmalloc (flags->n_iterations, sizeof *passarray);

  size = flags->size;
  if (size == -1)
    {
      /* Accept a length of zero only if it's a regular file.
         For any other type of file, try to get the size another way.  */
      if (S_ISREG (st.st_mode))
        {
          size = st.st_size;
          if (size < 0)
            {
              error (0, 0, _("%s: file has negative size"), qname);
              return false;
            }
        }
      else
        {
          size = lseek (fd, 0, SEEK_END);
          if (size <= 0)
            {
              /* We are unable to determine the length, up front.
                 Let dopass do that as part of its first iteration.  */
              size = -1;
            }
        }

      /* Allow `rounding up' only for regular files.  */
      if (0 <= size && !(flags->exact) && S_ISREG (st.st_mode))
        {
          size += ST_BLKSIZE (st) - 1 - (size - 1) % ST_BLKSIZE (st);

          /* If in rounding up, we've just overflowed, use the maximum.  */
          if (size < 0)
            size = TYPE_MAXIMUM (off_t);
        }
    }

  /* Schedule the passes in random order. */
  genpattern (passarray, flags->n_iterations, s);

  rs = randint_get_source (s);

  /* Do the work */
  for (i = 0; i < flags->n_iterations; i++)
    {
      int err = dopass (fd, qname, &size, passarray[i], rs, i + 1, n);
      if (err)
        {
          if (err < 0)
            {
              memset (passarray, 0, flags->n_iterations * sizeof (int));
              free (passarray);
              return false;
            }
          ok = false;
        }
    }

  memset (passarray, 0, flags->n_iterations * sizeof (int));
  free (passarray);

  if (flags->zero_fill)
    {
      int err = dopass (fd, qname, &size, 0, rs, flags->n_iterations + 1, n);
      if (err)
        {
          if (err < 0)
            return false;
          ok = false;
        }
    }

  /* Okay, now deallocate the data.  The effect of ftruncate on
     non-regular files is unspecified, so don't worry about any
     errors reported for them.  */
  if (flags->remove_file && ftruncate (fd, 0) != 0
      && S_ISREG (st.st_mode))
    {
      error (0, errno, _("%s: error truncating"), qname);
      return false;
    }

  return ok;
}

