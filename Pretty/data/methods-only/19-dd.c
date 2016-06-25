static void
scanargs (int argc, char *const *argv)
{
  int i;
  size_t blocksize = 0;

  for (i = optind; i < argc; i++)
    {
      char const *name = argv[i];
      char const *val = strchr (name, '=');

      if (val == NULL)
        {
          error (0, 0, _("unrecognized operand %s"), quote (name));
          usage (EXIT_FAILURE);
        }
      val++;

      if (operand_is (name, "if"))
        input_file = val;
      else if (operand_is (name, "of"))
        output_file = val;
      else if (operand_is (name, "conv"))
        conversions_mask |= parse_symbols (val, conversions,
                                           N_("invalid conversion"));
      else if (operand_is (name, "iflag"))
        input_flags |= parse_symbols (val, flags,
                                      N_("invalid input flag"));
      else if (operand_is (name, "oflag"))
        output_flags |= parse_symbols (val, flags,
                                       N_("invalid output flag"));
      else if (operand_is (name, "status"))
        status_flags |= parse_symbols (val, statuses,
                                       N_("invalid status flag"));
      else
        {
          bool invalid = false;
          uintmax_t n = parse_integer (val, &invalid);

          if (operand_is (name, "ibs"))
            {
              invalid |= ! (0 < n && n <= MAX_BLOCKSIZE (INPUT_BLOCK_SLOP));
              input_blocksize = n;
            }
          else if (operand_is (name, "obs"))
            {
              invalid |= ! (0 < n && n <= MAX_BLOCKSIZE (OUTPUT_BLOCK_SLOP));
              output_blocksize = n;
            }
          else if (operand_is (name, "bs"))
            {
              invalid |= ! (0 < n && n <= MAX_BLOCKSIZE (INPUT_BLOCK_SLOP));
              blocksize = n;
            }
          else if (operand_is (name, "cbs"))
            {
              invalid |= ! (0 < n && n <= SIZE_MAX);
              conversion_blocksize = n;
            }
          else if (operand_is (name, "skip"))
            skip_records = n;
          else if (operand_is (name, "seek"))
            seek_records = n;
          else if (operand_is (name, "count"))
            max_records = n;
          else
            {
              error (0, 0, _("unrecognized operand %s"), quote (name));
              usage (EXIT_FAILURE);
            }

          if (invalid)
            error (EXIT_FAILURE, 0, _("invalid number %s"), quote (val));
        }
    }

  if (blocksize)
    input_blocksize = output_blocksize = blocksize;
  else
    {
      /* POSIX says dd aggregates short reads into
         output_blocksize if bs= is not specified.  */
      conversions_mask |= C_TWOBUFS;
    }

  if (input_blocksize == 0)
    input_blocksize = DEFAULT_BLOCKSIZE;
  if (output_blocksize == 0)
    output_blocksize = DEFAULT_BLOCKSIZE;
  if (conversion_blocksize == 0)
    conversions_mask &= ~(C_BLOCK | C_UNBLOCK);

  if (input_flags & (O_DSYNC | O_SYNC))
    input_flags |= O_RSYNC;

  if (output_flags & O_FULLBLOCK)
    {
      error (0, 0, "%s: %s", _("invalid output flag"), "'fullblock'");
      usage (EXIT_FAILURE);
    }
  iread_fnc = ((input_flags & O_FULLBLOCK)
               ? iread_fullblock
               : iread);
  input_flags &= ~O_FULLBLOCK;

  if (multiple_bits_set (conversions_mask & (C_ASCII | C_EBCDIC | C_IBM)))
    error (EXIT_FAILURE, 0, _("cannot combine any two of {ascii,ebcdic,ibm}"));
  if (multiple_bits_set (conversions_mask & (C_BLOCK | C_UNBLOCK)))
    error (EXIT_FAILURE, 0, _("cannot combine block and unblock"));
  if (multiple_bits_set (conversions_mask & (C_LCASE | C_UCASE)))
    error (EXIT_FAILURE, 0, _("cannot combine lcase and ucase"));
  if (multiple_bits_set (conversions_mask & (C_EXCL | C_NOCREAT)))
    error (EXIT_FAILURE, 0, _("cannot combine excl and nocreat"));
}

