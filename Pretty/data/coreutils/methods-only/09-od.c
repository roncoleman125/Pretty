/* Given a list of one or more input filenames FILE_LIST, set the global
   file pointer IN_STREAM to position N_SKIP in the concatenation of
   those files.  If any file operation fails or if there are fewer than
   N_SKIP bytes in the combined input, give an error message and return
   false.  When possible, use seek rather than read operations to
   advance IN_STREAM.  */

static bool
skip (uintmax_t n_skip)
{
  bool ok = true;
  int in_errno = 0;

  if (n_skip == 0)
    return true;

  while (in_stream != NULL)	/* EOF.  */
    {
      struct stat file_stats;

      /* First try seeking.  For large offsets, this extra work is
         worthwhile.  If the offset is below some threshold it may be
         more efficient to move the pointer by reading.  There are two
         issues when trying to seek:
           - the file must be seekable.
           - before seeking to the specified position, make sure
             that the new position is in the current file.
             Try to do that by getting file's size using fstat.
             But that will work only for regular files.  */

      if (fstat (fileno (in_stream), &file_stats) == 0)
        {
          /* The st_size field is valid only for regular files
             (and for symbolic links, which cannot occur here).
             If the number of bytes left to skip is larger than
             the size of the current file, we can decrement n_skip
             and go on to the next file.  Skip this optimization also
             when st_size is 0, because some kernels report that
             nonempty files in /proc have st_size == 0.  */
          if (S_ISREG (file_stats.st_mode) && 0 < file_stats.st_size)
            {
              if ((uintmax_t) file_stats.st_size < n_skip)
                n_skip -= file_stats.st_size;
              else
                {
                  if (fseeko (in_stream, n_skip, SEEK_CUR) != 0)
                    {
                      in_errno = errno;
                      ok = false;
                    }
                  n_skip = 0;
                }
            }

          /* If it's not a regular file with nonnegative size,
             position the file pointer by reading.  */

          else
            {
              char buf[BUFSIZ];
              size_t n_bytes_read, n_bytes_to_read = BUFSIZ;

              while (0 < n_skip)
                {
                  if (n_skip < n_bytes_to_read)
                    n_bytes_to_read = n_skip;
                  n_bytes_read = fread (buf, 1, n_bytes_to_read, in_stream);
                  n_skip -= n_bytes_read;
                  if (n_bytes_read != n_bytes_to_read)
                    {
                      in_errno = errno;
                      ok = false;
                      n_skip = 0;
                      break;
                    }
                }
            }

          if (n_skip == 0)
            break;
        }

      else   /* cannot fstat() file */
        {
          error (0, errno, "%s", input_filename);
          ok = false;
        }

      ok &= check_and_close (in_errno);

      ok &= open_next_file ();
    }

  if (n_skip != 0)
    error (EXIT_FAILURE, 0, _("cannot skip past end of combined input"));

  return ok;
}

