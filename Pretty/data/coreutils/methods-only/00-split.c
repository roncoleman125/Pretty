static void
set_suffix_length (uintmax_t n_units, enum Split_type split_type)
{

  size_t suffix_needed = 0;

  /* Auto-calculate the suffix length if the number of files is given.  */
  if (split_type == type_chunk_bytes || split_type == type_chunk_lines
      || split_type == type_rr)
    {
      size_t alphabet_len = strlen (suffix_alphabet);
      bool alphabet_slop = (n_units % alphabet_len) != 0;
      while (n_units /= alphabet_len)
        suffix_needed++;
      suffix_needed += alphabet_slop;
    }

  if (suffix_length)            /* set by user */
    {
      if (suffix_length < suffix_needed)
        {
          error (EXIT_FAILURE, 0,
                 _("the suffix length needs to be at least %zu"),
                 suffix_needed);
        }
      return;
    }
  else
    suffix_length = MAX (DEFAULT_SUFFIX_LENGTH, suffix_needed);
}

