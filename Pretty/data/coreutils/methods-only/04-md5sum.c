/* An interface to the function, DIGEST_STREAM.
   Operate on FILENAME (it may be "-").

   *BINARY indicates whether the file is binary.  BINARY < 0 means it
   depends on whether binary mode makes any difference and the file is
   a terminal; in that case, clear *BINARY if the file was treated as
   text because it was a terminal.

   Put the checksum in *BIN_RESULT, which must be properly aligned.
   Return true if successful.  */

static bool
digest_file (const char *filename, int *binary, unsigned char *bin_result)
{
  FILE *fp;
  int err;
  bool is_stdin = STREQ (filename, "-");

  if (is_stdin)
    {
      have_read_stdin = true;
      fp = stdin;
      if (O_BINARY && *binary)
        {
          if (*binary < 0)
            *binary = ! isatty (STDIN_FILENO);
          if (*binary)
            xfreopen (NULL, "rb", stdin);
        }
    }
  else
    {
      fp = fopen (filename, (O_BINARY && *binary ? "rb" : "r"));
      if (fp == NULL)
        {
          error (0, errno, "%s", filename);
          return false;
        }
    }

  fadvise (fp, FADVISE_SEQUENTIAL);

  err = DIGEST_STREAM (fp, bin_result);
  if (err)
    {
      error (0, errno, "%s", filename);
      if (fp != stdin)
        fclose (fp);
      return false;
    }

  if (!is_stdin && fclose (fp) != 0)
    {
      error (0, errno, "%s", filename);
      return false;
    }

  return true;
}

