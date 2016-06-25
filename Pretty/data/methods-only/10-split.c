/* Rotate file descriptors when we're writing to more output files than we
   have available file descriptors.
   Return whether we came under file resource pressure.
   If so, it's probably best to close each file when finished with it.  */

static bool
ofile_open (of_t *files, size_t i_check, size_t nfiles)
{
  bool file_limit = false;

  if (files[i_check].ofd <= OFD_NEW)
    {
      int fd;
      size_t i_reopen = i_check ? i_check - 1 : nfiles - 1;

      /* Another process could have opened a file in between the calls to
         close and open, so we should keep trying until open succeeds or
         we've closed all of our files.  */
      while (true)
        {
          if (files[i_check].ofd == OFD_NEW)
            fd = create (files[i_check].of_name);
          else /* OFD_APPEND  */
            {
              /* Attempt to append to previously opened file.
                 We use O_NONBLOCK to support writing to fifos,
                 where the other end has closed because of our
                 previous close.  In that case we'll immediately
                 get an error, rather than waiting indefinitely.
                 In specialised cases the consumer can keep reading
                 from the fifo, terminating on conditions in the data
                 itself, or perhaps never in the case of `tail -f`.
                 I.E. for fifos it is valid to attempt this reopen.  */
               fd = open (files[i_check].of_name,
                          O_WRONLY | O_BINARY | O_APPEND | O_NONBLOCK);
             }

          if (-1 < fd)
            break;

          if (!(errno == EMFILE || errno == ENFILE))
            error (EXIT_FAILURE, errno, "%s", files[i_check].of_name);

          file_limit = true;

          /* Search backwards for an open file to close.  */
          while (files[i_reopen].ofd < 0)
            {
              i_reopen = i_reopen ? i_reopen - 1 : nfiles - 1;
              /* No more open files to close, exit with E[NM]FILE.  */
              if (i_reopen == i_check)
                error (EXIT_FAILURE, errno, "%s", files[i_check].of_name);
            }

          if (fclose (files[i_reopen].ofile) != 0)
            error (EXIT_FAILURE, errno, "%s", files[i_reopen].of_name);
          files[i_reopen].ofd = OFD_APPEND;
        }

      files[i_check].ofd = fd;
      if (!(files[i_check].ofile = fdopen (fd, "a")))
        error (EXIT_FAILURE, errno, "%s", files[i_check].of_name);
    }

  return file_limit;
}

