/* Write N_BYTES bytes from CURR_BLOCK to standard output once for each
   of the N_SPEC format specs.  CURRENT_OFFSET is the byte address of
   CURR_BLOCK in the concatenation of input files, and it is printed
   (optionally) only before the output line associated with the first
   format spec.  When duplicate blocks are being abbreviated, the output
   for a sequence of identical input blocks is the output for the first
   block followed by an asterisk alone on a line.  It is valid to compare
   the blocks PREV_BLOCK and CURR_BLOCK only when N_BYTES == BYTES_PER_BLOCK.
   That condition may be false only for the last input block.  */

static void
write_block (uintmax_t current_offset, size_t n_bytes,
             const char *prev_block, const char *curr_block)
{
  static bool first = true;
  static bool prev_pair_equal = false;


  if (abbreviate_duplicate_blocks
      && !first && n_bytes == bytes_per_block
      && EQUAL_BLOCKS (prev_block, curr_block))
    {
      if (prev_pair_equal)
        {
          /* The two preceding blocks were equal, and the current
             block is the same as the last one, so print nothing.  */
        }
      else
        {
          printf ("*\n");
          prev_pair_equal = true;
        }
    }
  else
    {
      size_t i;

      prev_pair_equal = false;
      for (i = 0; i < n_specs; i++)
        {
          int datum_width = width_bytes[spec[i].size];
          int fields_per_block = bytes_per_block / datum_width;
          int blank_fields = (bytes_per_block - n_bytes) / datum_width;
          if (i == 0)
            format_address (current_offset, '\0');
          else
            printf ("%*s", address_pad_len, "");
          (*spec[i].print_function) (fields_per_block, blank_fields,
                                     curr_block, spec[i].fmt_string,
                                     spec[i].field_width, spec[i].pad_width);
          if (spec[i].hexl_mode_trailer)
            {
              /* space-pad out to full line width, then dump the trailer */
              int field_width = spec[i].field_width;
              int pad_width = (spec[i].pad_width * blank_fields
                               / fields_per_block);
              printf ("%*s", blank_fields * field_width + pad_width, "");
              dump_hexl_mode_trailer (n_bytes, curr_block);
            }
          putchar ('\n');
        }
    }
  first = false;
}

