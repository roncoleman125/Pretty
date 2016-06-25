/* Copy the regular file open on SRC_FD/SRC_NAME to DST_FD/DST_NAME,
   honoring the MAKE_HOLES setting and using the BUF_SIZE-byte buffer
   BUF for temporary storage.  Copy no more than MAX_N_READ bytes.
   Return true upon successful completion;
   print a diagnostic and return false upon error.
   Note that for best results, BUF should be "well"-aligned.
   BUF must have sizeof(uintptr_t)-1 bytes of additional space
   beyond BUF[BUF_SIZE-1].
   Set *LAST_WRITE_MADE_HOLE to true if the final operation on
   DEST_FD introduced a hole.  Set *TOTAL_N_READ to the number of
   bytes read.  */
static bool
sparse_copy (int src_fd, int dest_fd, char *buf, size_t buf_size,
             bool make_holes,
             char const *src_name, char const *dst_name,
             uintmax_t max_n_read, off_t *total_n_read,
             bool *last_write_made_hole)
{
  typedef uintptr_t word;
  *last_write_made_hole = false;
  *total_n_read = 0;

  while (max_n_read)
    {
      word *wp = NULL;

      ssize_t n_read = read (src_fd, buf, MIN (max_n_read, buf_size));
      if (n_read < 0)
        {
          if (errno == EINTR)
            continue;
          error (0, errno, _("reading %s"), quote (src_name));
          return false;
        }
      if (n_read == 0)
        break;
      max_n_read -= n_read;
      *total_n_read += n_read;

      if (make_holes)
        {
          char *cp;

          /* Sentinel to stop loop.  */
          buf[n_read] = '\1';
          /* Usually, buf[n_read] is not the byte just before a "word"
             (aka uintptr_t) boundary.  In that case, the word-oriented
             test below (*wp++ == 0) would read some uninitialized bytes
             after the sentinel.  To avoid false-positive reports about
             this condition (e.g., from a tool like valgrind), set the
             remaining bytes -- to any value.  */
          memset (buf + n_read + 1, 0, sizeof (word) - 1);

          /* Find first nonzero *word*, or the word with the sentinel.  */

          wp = (word *) buf;
          while (*wp++ == 0)
            continue;

          /* Find the first nonzero *byte*, or the sentinel.  */

          cp = (char *) (wp - 1);
          while (*cp++ == 0)
            continue;

          if (cp <= buf + n_read)
            /* Clear to indicate that a normal write is needed. */
            wp = NULL;
          else
            {
              /* We found the sentinel, so the whole input block was zero.
                 Make a hole.  */
              if (lseek (dest_fd, n_read, SEEK_CUR) < 0)
                {
                  error (0, errno, _("cannot lseek %s"), quote (dst_name));
                  return false;
                }
              *last_write_made_hole = true;
            }
        }

      if (!wp)
        {
          size_t n = n_read;
          if (full_write (dest_fd, buf, n) != n)
            {
              error (0, errno, _("writing %s"), quote (dst_name));
              return false;
            }
          *last_write_made_hole = false;

          /* It is tempting to return early here upon a short read from a
             regular file.  That would save the final read syscall for each
             file.  Unfortunately that doesn't work for certain files in
             /proc with linux kernels from at least 2.6.9 .. 2.6.29.  */
        }
    }

  return true;
}

