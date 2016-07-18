/* Return the sort buffer size to use with the input files identified
   by FPS and FILES, which are alternate names of the same files.
   NFILES gives the number of input files; NFPS may be less.  Assume
   that each input line requires LINE_BYTES extra bytes' worth of line
   information.  Do not exceed the size bound specified by the user
   (or a default size bound, if the user does not specify one).  */

static size_t
sort_buffer_size (FILE *const *fps, size_t nfps,
                  char *const *files, size_t nfiles,
                  size_t line_bytes)
{
  /* A bound on the input size.  If zero, the bound hasn't been
     determined yet.  */
  static size_t size_bound;

  /* In the worst case, each input byte is a newline.  */
  size_t worst_case_per_input_byte = line_bytes + 1;

  /* Keep enough room for one extra input line and an extra byte.
     This extra room might be needed when preparing to read EOF.  */
  size_t size = worst_case_per_input_byte + 1;

  size_t i;

  for (i = 0; i < nfiles; i++)
    {
      struct stat st;
      off_t file_size;
      size_t worst_case;

      if ((i < nfps ? fstat (fileno (fps[i]), &st)
           : STREQ (files[i], "-") ? fstat (STDIN_FILENO, &st)
           : stat (files[i], &st))
          != 0)
        die (_("stat failed"), files[i]);

      if (S_ISREG (st.st_mode))
        file_size = st.st_size;
      else
        {
          /* The file has unknown size.  If the user specified a sort
             buffer size, use that; otherwise, guess the size.  */
          if (sort_size)
            return sort_size;
          file_size = INPUT_FILE_SIZE_GUESS;
        }

      if (! size_bound)
        {
          size_bound = sort_size;
          if (! size_bound)
            size_bound = default_sort_size ();
        }

      /* Add the amount of memory needed to represent the worst case
         where the input consists entirely of newlines followed by a
         single non-newline.  Check for overflow.  */
      worst_case = file_size * worst_case_per_input_byte + 1;
      if (file_size != worst_case / worst_case_per_input_byte
          || size_bound - size <= worst_case)
        return size_bound;
      size += worst_case;
    }

  return size;
}

