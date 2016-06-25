/* Throw away RECORDS blocks of BLOCKSIZE bytes on file descriptor FDESC,
   which is open with read permission for FILE.  Store up to BLOCKSIZE
   bytes of the data at a time in BUF, if necessary.  RECORDS must be
   nonzero.  If fdesc is STDIN_FILENO, advance the input offset.
   Return the number of records remaining, i.e., that were not skipped
   because EOF was reached.  */

static uintmax_t
skip (int fdesc, char const *file, uintmax_t records, size_t blocksize,
      char *buf)
{
  uintmax_t offset = records * blocksize;

  /* Try lseek and if an error indicates it was an inappropriate operation --
     or if the file offset is not representable as an off_t --
     fall back on using read.  */

  errno = 0;
  if (records <= OFF_T_MAX / blocksize
      && 0 <= skip_via_lseek (file, fdesc, offset, SEEK_CUR))
    {
      if (fdesc == STDIN_FILENO)
        {
           struct stat st;
           if (fstat (STDIN_FILENO, &st) != 0)
             error (EXIT_FAILURE, errno, _("cannot fstat %s"), quote (file));
           if (S_ISREG (st.st_mode) && st.st_size < (input_offset + offset))
             {
               /* When skipping past EOF, return the number of _full_ blocks
                * that are not skipped, and set offset to EOF, so the caller
                * can determine the requested skip was not satisfied.  */
               records = ( offset - st.st_size ) / blocksize;
               offset = st.st_size - input_offset;
             }
           else
             records = 0;
           advance_input_offset (offset);
        }
      else
        records = 0;
      return records;
    }
  else
    {
      int lseek_errno = errno;

      /* The seek request may have failed above if it was too big
         (> device size, > max file size, etc.)
         Or it may not have been done at all (> OFF_T_MAX).
         Therefore try to seek to the end of the file,
         to avoid redundant reading.  */
      if ((skip_via_lseek (file, fdesc, 0, SEEK_END)) >= 0)
        {
          /* File is seekable, and we're at the end of it, and
             size <= OFF_T_MAX. So there's no point using read to advance.  */

          if (!lseek_errno)
            {
              /* The original seek was not attempted as offset > OFF_T_MAX.
                 We should error for write as can't get to the desired
                 location, even if OFF_T_MAX < max file size.
                 For read we're not going to read any data anyway,
                 so we should error for consistency.
                 It would be nice to not error for /dev/{zero,null}
                 for any offset, but that's not a significant issue.  */
              lseek_errno = EOVERFLOW;
            }

          if (fdesc == STDIN_FILENO)
            error (0, lseek_errno, _("%s: cannot skip"), quote (file));
          else
            error (0, lseek_errno, _("%s: cannot seek"), quote (file));
          /* If the file has a specific size and we've asked
             to skip/seek beyond the max allowable, then quit.  */
          quit (EXIT_FAILURE);
        }
      /* else file_size && offset > OFF_T_MAX or file ! seekable */

      do
        {
          ssize_t nread = iread_fnc (fdesc, buf, blocksize);
          if (nread < 0)
            {
              if (fdesc == STDIN_FILENO)
                {
                  error (0, errno, _("reading %s"), quote (file));
                  if (conversions_mask & C_NOERROR)
                    {
                      print_stats ();
                      continue;
                    }
                }
              else
                error (0, lseek_errno, _("%s: cannot seek"), quote (file));
              quit (EXIT_FAILURE);
            }

          if (nread == 0)
            break;
          if (fdesc == STDIN_FILENO)
            advance_input_offset (nread);
        }
      while (--records != 0);

      return records;
    }
}

