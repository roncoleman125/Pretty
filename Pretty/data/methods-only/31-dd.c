/* The main loop.  */

static int
dd_copy (void)
{
  char *ibuf, *bufstart;	/* Input buffer. */
  /* These are declared static so that even though we don't free the
     buffers, valgrind will recognize that there is no "real" leak.  */
  static char *real_buf;	/* real buffer address before alignment */
  static char *real_obuf;
  ssize_t nread;		/* Bytes read in the current block.  */

  /* If nonzero, then the previously read block was partial and
     PARTREAD was its size.  */
  size_t partread = 0;

  int exit_status = EXIT_SUCCESS;
  size_t n_bytes_read;

  /* Leave at least one extra byte at the beginning and end of `ibuf'
     for conv=swab, but keep the buffer address even.  But some peculiar
     device drivers work only with word-aligned buffers, so leave an
     extra two bytes.  */

  /* Some devices require alignment on a sector or page boundary
     (e.g. character disk devices).  Align the input buffer to a
     page boundary to cover all bases.  Note that due to the swab
     algorithm, we must have at least one byte in the page before
     the input buffer;  thus we allocate 2 pages of slop in the
     real buffer.  8k above the blocksize shouldn't bother anyone.

     The page alignment is necessary on any Linux kernel that supports
     either the SGI raw I/O patch or Steven Tweedies raw I/O patch.
     It is necessary when accessing raw (i.e. character special) disk
     devices on Unixware or other SVR4-derived system.  */

  real_buf = malloc (input_blocksize + INPUT_BLOCK_SLOP);
  if (!real_buf)
    error (EXIT_FAILURE, 0,
           _("memory exhausted by input buffer of size %zu bytes (%s)"),
           input_blocksize, human_size (input_blocksize));

  ibuf = real_buf;
  ibuf += SWAB_ALIGN_OFFSET;	/* allow space for swab */

  ibuf = ptr_align (ibuf, page_size);

  if (conversions_mask & C_TWOBUFS)
    {
      /* Page-align the output buffer, too.  */
      real_obuf = malloc (output_blocksize + OUTPUT_BLOCK_SLOP);
      if (!real_obuf)
        error (EXIT_FAILURE, 0,
               _("memory exhausted by output buffer of size %zu bytes (%s)"),
               output_blocksize, human_size (output_blocksize));
      obuf = ptr_align (real_obuf, page_size);
    }
  else
    {
      real_obuf = NULL;
      obuf = ibuf;
    }

  if (skip_records != 0)
    {
      uintmax_t us_bytes = input_offset + (skip_records * input_blocksize);
      uintmax_t us_blocks = skip (STDIN_FILENO, input_file,
                                  skip_records, input_blocksize, ibuf);
      us_bytes -= input_offset;

      /* POSIX doesn't say what to do when dd detects it has been
         asked to skip past EOF, so I assume it's non-fatal.
         There are 3 reasons why there might be unskipped blocks/bytes:
             1. file is too small
             2. pipe has not enough data
             3. short reads  */
      if (us_blocks || (!input_offset_overflow && us_bytes))
        {
          error (0, 0,
                 _("%s: cannot skip to specified offset"), quote (input_file));
        }
    }

  if (seek_records != 0)
    {
      uintmax_t write_records = skip (STDOUT_FILENO, output_file,
                                      seek_records, output_blocksize, obuf);

      if (write_records != 0)
        {
          memset (obuf, 0, output_blocksize);

          do
            if (iwrite (STDOUT_FILENO, obuf, output_blocksize)
                != output_blocksize)
              {
                error (0, errno, _("writing to %s"), quote (output_file));
                quit (EXIT_FAILURE);
              }
          while (--write_records != 0);
        }
    }

  if (max_records == 0)
    return exit_status;

  while (1)
    {
      if (r_partial + r_full >= max_records)
        break;

      /* Zero the buffer before reading, so that if we get a read error,
         whatever data we are able to read is followed by zeros.
         This minimizes data loss. */
      if ((conversions_mask & C_SYNC) && (conversions_mask & C_NOERROR))
        memset (ibuf,
                (conversions_mask & (C_BLOCK | C_UNBLOCK)) ? ' ' : '\0',
                input_blocksize);

      nread = iread_fnc (STDIN_FILENO, ibuf, input_blocksize);

      if (nread == 0)
        break;			/* EOF.  */

      if (nread < 0)
        {
          error (0, errno, _("reading %s"), quote (input_file));
          if (conversions_mask & C_NOERROR)
            {
              print_stats ();
              /* Seek past the bad block if possible. */
              if (!advance_input_after_read_error (input_blocksize - partread))
                {
                  exit_status = EXIT_FAILURE;

                  /* Suppress duplicate diagnostics.  */
                  input_seekable = false;
                  input_seek_errno = ESPIPE;
                }
              if ((conversions_mask & C_SYNC) && !partread)
                /* Replace the missing input with null bytes and
                   proceed normally.  */
                nread = 0;
              else
                continue;
            }
          else
            {
              /* Write any partial block. */
              exit_status = EXIT_FAILURE;
              break;
            }
        }

      n_bytes_read = nread;
      advance_input_offset (nread);

      if (n_bytes_read < input_blocksize)
        {
          r_partial++;
          partread = n_bytes_read;
          if (conversions_mask & C_SYNC)
            {
              if (!(conversions_mask & C_NOERROR))
                /* If C_NOERROR, we zeroed the block before reading. */
                memset (ibuf + n_bytes_read,
                        (conversions_mask & (C_BLOCK | C_UNBLOCK)) ? ' ' : '\0',
                        input_blocksize - n_bytes_read);
              n_bytes_read = input_blocksize;
            }
        }
      else
        {
          r_full++;
          partread = 0;
        }

      if (ibuf == obuf)		/* If not C_TWOBUFS. */
        {
          size_t nwritten = iwrite (STDOUT_FILENO, obuf, n_bytes_read);
          w_bytes += nwritten;
          if (nwritten != n_bytes_read)
            {
              error (0, errno, _("writing %s"), quote (output_file));
              return EXIT_FAILURE;
            }
          else if (n_bytes_read == input_blocksize)
            w_full++;
          else
            w_partial++;
          continue;
        }

      /* Do any translations on the whole buffer at once.  */

      if (translation_needed)
        translate_buffer (ibuf, n_bytes_read);

      if (conversions_mask & C_SWAB)
        bufstart = swab_buffer (ibuf, &n_bytes_read);
      else
        bufstart = ibuf;

      if (conversions_mask & C_BLOCK)
        copy_with_block (bufstart, n_bytes_read);
      else if (conversions_mask & C_UNBLOCK)
        copy_with_unblock (bufstart, n_bytes_read);
      else
        copy_simple (bufstart, n_bytes_read);
    }

  /* If we have a char left as a result of conv=swab, output it.  */
  if (char_is_saved)
    {
      if (conversions_mask & C_BLOCK)
        copy_with_block (&saved_char, 1);
      else if (conversions_mask & C_UNBLOCK)
        copy_with_unblock (&saved_char, 1);
      else
        output_char (saved_char);
    }

  if ((conversions_mask & C_BLOCK) && col > 0)
    {
      /* If the final input line didn't end with a '\n', pad
         the output block to `conversion_blocksize' chars.  */
      size_t i;
      for (i = col; i < conversion_blocksize; i++)
        output_char (space_character);
    }

  if (col && (conversions_mask & C_UNBLOCK))
    {
      /* If there was any output, add a final '\n'.  */
      output_char (newline_character);
    }

  /* Write out the last block. */
  if (oc != 0)
    {
      size_t nwritten = iwrite (STDOUT_FILENO, obuf, oc);
      w_bytes += nwritten;
      if (nwritten != 0)
        w_partial++;
      if (nwritten != oc)
        {
          error (0, errno, _("writing %s"), quote (output_file));
          return EXIT_FAILURE;
        }
    }

  if ((conversions_mask & C_FDATASYNC) && fdatasync (STDOUT_FILENO) != 0)
    {
      if (errno != ENOSYS && errno != EINVAL)
        {
          error (0, errno, _("fdatasync failed for %s"), quote (output_file));
          exit_status = EXIT_FAILURE;
        }
      conversions_mask |= C_FSYNC;
    }

  if (conversions_mask & C_FSYNC)
    while (fsync (STDOUT_FILENO) != 0)
      if (errno != EINTR)
        {
          error (0, errno, _("fsync failed for %s"), quote (output_file));
          return EXIT_FAILURE;
        }

  return exit_status;
}

