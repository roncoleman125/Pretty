/* Read a chunk of size BYTES_PER_BLOCK from the input files, write the
   formatted block to standard output, and repeat until the specified
   maximum number of bytes has been read or until all input has been
   processed.  If the last block read is smaller than BYTES_PER_BLOCK
   and its size is not a multiple of the size associated with a format
   spec, extend the input block with zero bytes until its length is a
   multiple of all format spec sizes.  Write the final block.  Finally,
   write on a line by itself the offset of the byte after the last byte
   read.  Accumulate return values from calls to read_block and
   check_and_close, and if any was false, return false.
   Otherwise, return true.  */

static bool
dump (void)
{
  char *block[2];
  uintmax_t current_offset;
  bool idx = false;
  bool ok = true;
  size_t n_bytes_read;

  block[0] = xnmalloc (2, bytes_per_block);
  block[1] = block[0] + bytes_per_block;

  current_offset = n_bytes_to_skip;

  if (limit_bytes_to_format)
    {
      while (1)
        {
          size_t n_needed;
          if (current_offset >= end_offset)
            {
              n_bytes_read = 0;
              break;
            }
          n_needed = MIN (end_offset - current_offset,
                          (uintmax_t) bytes_per_block);
          ok &= read_block (n_needed, block[idx], &n_bytes_read);
          if (n_bytes_read < bytes_per_block)
            break;
          assert (n_bytes_read == bytes_per_block);
          write_block (current_offset, n_bytes_read,
                       block[!idx], block[idx]);
          current_offset += n_bytes_read;
          idx = !idx;
        }
    }
  else
    {
      while (1)
        {
          ok &= read_block (bytes_per_block, block[idx], &n_bytes_read);
          if (n_bytes_read < bytes_per_block)
            break;
          assert (n_bytes_read == bytes_per_block);
          write_block (current_offset, n_bytes_read,
                       block[!idx], block[idx]);
          current_offset += n_bytes_read;
          idx = !idx;
        }
    }

  if (n_bytes_read > 0)
    {
      int l_c_m;
      size_t bytes_to_write;

      l_c_m = get_lcm ();

      /* Ensure zero-byte padding up to the smallest multiple of l_c_m that
         is at least as large as n_bytes_read.  */
      bytes_to_write = l_c_m * ((n_bytes_read + l_c_m - 1) / l_c_m);

      memset (block[idx] + n_bytes_read, 0, bytes_to_write - n_bytes_read);
      write_block (current_offset, n_bytes_read, block[!idx], block[idx]);
      current_offset += n_bytes_read;
    }

  format_address (current_offset, '\n');

  if (limit_bytes_to_format && current_offset >= end_offset)
    ok &= check_and_close (0);

  free (block[0]);

  return ok;
}

