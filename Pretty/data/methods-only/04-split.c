/* Write BYTES bytes at BP to an output file.
   If NEW_FILE_FLAG is true, open the next output file.
   Otherwise add to the same output file already in use.  */

static void
cwrite (bool new_file_flag, const char *bp, size_t bytes)
{
  if (new_file_flag)
    {
      if (!bp && bytes == 0 && elide_empty_files)
        return;
      if (output_desc >= 0 && close (output_desc) < 0)
        error (EXIT_FAILURE, errno, "%s", outfile);
      next_file_name ();
      if ((output_desc = create (outfile)) < 0)
        error (EXIT_FAILURE, errno, "%s", outfile);
    }
  if (full_write (output_desc, bp, bytes) != bytes)
    error (EXIT_FAILURE, errno, "%s", outfile);
}

