/* Compute the next sequential output file name and store it into the
   string `outfile'.  */

static void
next_file_name (void)
{
  /* Index in suffix_alphabet of each character in the suffix.  */
  static size_t *sufindex;

  if (! outfile)
    {
      /* Allocate and initialize the first file name.  */

      size_t outbase_length = strlen (outbase);
      size_t outfile_length = outbase_length + suffix_length;
      if (outfile_length + 1 < outbase_length)
        xalloc_die ();
      outfile = xmalloc (outfile_length + 1);
      outfile_mid = outfile + outbase_length;
      memcpy (outfile, outbase, outbase_length);
      memset (outfile_mid, suffix_alphabet[0], suffix_length);
      outfile[outfile_length] = 0;
      sufindex = xcalloc (suffix_length, sizeof *sufindex);

      /* POSIX requires that if the output file name is too long for
         its directory, `split' must fail without creating any files.
         This must be checked for explicitly on operating systems that
         silently truncate file names.  */
      {
        char *dir = dir_name (outfile);
        long name_max = pathconf (dir, _PC_NAME_MAX);
        if (0 <= name_max && name_max < base_len (last_component (outfile)))
          error (EXIT_FAILURE, ENAMETOOLONG, "%s", outfile);
        free (dir);
      }
    }
  else
    {
      /* Increment the suffix in place, if possible.  */

      size_t i = suffix_length;
      while (i-- != 0)
        {
          sufindex[i]++;
          outfile_mid[i] = suffix_alphabet[sufindex[i]];
          if (outfile_mid[i])
            return;
          sufindex[i] = 0;
          outfile_mid[i] = suffix_alphabet[sufindex[i]];
        }
      error (EXIT_FAILURE, 0, _("output file suffixes exhausted"));
    }
}

