/* Perform an efficient extent copy, if possible.  This avoids
   the overhead of detecting holes in hole-introducing/preserving
   copy, and thus makes copying sparse files much more efficient.
   Upon a successful copy, return true.  If the initial extent scan
   fails, set *NORMAL_COPY_REQUIRED to true and return false.
   Upon any other failure, set *NORMAL_COPY_REQUIRED to false and
   return false.  */
static bool
extent_copy (int src_fd, int dest_fd, char *buf, size_t buf_size,
             off_t src_total_size, bool make_holes,
             char const *src_name, char const *dst_name,
             bool *require_normal_copy)
{
  struct extent_scan scan;
  off_t last_ext_start = 0;
  uint64_t last_ext_len = 0;

  /* Keep track of the output position.
     We may need this at the end, for a final ftruncate.  */
  off_t dest_pos = 0;

  extent_scan_init (src_fd, &scan);

  *require_normal_copy = false;
  bool wrote_hole_at_eof = true;
  do
    {
      bool ok = extent_scan_read (&scan);
      if (! ok)
        {
          if (scan.hit_final_extent)
            break;

          if (scan.initial_scan_failed)
            {
              *require_normal_copy = true;
              return false;
            }

          error (0, errno, _("%s: failed to get extents info"),
                 quote (src_name));
          return false;
        }

      unsigned int i;
      for (i = 0; i < scan.ei_count; i++)
        {
          off_t ext_start = scan.ext_info[i].ext_logical;
          uint64_t ext_len = scan.ext_info[i].ext_length;

          if (lseek (src_fd, ext_start, SEEK_SET) < 0)
            {
              error (0, errno, _("cannot lseek %s"), quote (src_name));
            fail:
              extent_scan_free (&scan);
              return false;
            }

          if (make_holes)
            {
              if (lseek (dest_fd, ext_start, SEEK_SET) < 0)
                {
                  error (0, errno, _("cannot lseek %s"), quote (dst_name));
                  goto fail;
                }
            }
          else
            {
              /* When not inducing holes and when there is a hole between
                 the end of the previous extent and the beginning of the
                 current one, write zeros to the destination file.  */
              if (last_ext_start + last_ext_len < ext_start)
                {
                  uint64_t hole_size = (ext_start
                                        - last_ext_start
                                        - last_ext_len);
                  if (! write_zeros (dest_fd, hole_size))
                    {
                      error (0, errno, _("%s: write failed"), quote (dst_name));
                      goto fail;
                    }
                }
            }

          last_ext_start = ext_start;
          last_ext_len = ext_len;

          off_t n_read;
          if ( ! sparse_copy (src_fd, dest_fd, buf, buf_size,
                              make_holes, src_name, dst_name,
                              ext_len, &n_read,
                              &wrote_hole_at_eof))
            return false;

          dest_pos = ext_start + n_read;
        }

      /* Release the space allocated to scan->ext_info.  */
      extent_scan_free (&scan);

    }
  while (! scan.hit_final_extent);

  /* When the source file ends with a hole, we have to do a little more work,
     since the above copied only up to and including the final extent.
     In order to complete the copy, we may have to insert a hole or write
     zeros in the destination corresponding to the source file's hole-at-EOF.

     In addition, if the final extent was a block of zeros at EOF and we've
     just converted them to a hole in the destination, we must call ftruncate
     here in order to record the proper length in the destination.  */
  if ((dest_pos < src_total_size || wrote_hole_at_eof)
      && (make_holes
          ? ftruncate (dest_fd, src_total_size)
          : ! write_zeros (dest_fd, src_total_size - dest_pos)))
    {
      error (0, errno, _("failed to extend %s"), quote (dst_name));
      return false;
    }

  return true;
}

