static void
do_encode (FILE *in, FILE *out, uintmax_t wrap_column)
{
  size_t current_column = 0;
  char inbuf[BLOCKSIZE];
  char outbuf[B64BLOCKSIZE];
  size_t sum;

  do
    {
      size_t n;

      sum = 0;
      do
        {
          n = fread (inbuf + sum, 1, BLOCKSIZE - sum, in);
          sum += n;
        }
      while (!feof (in) && !ferror (in) && sum < BLOCKSIZE);

      if (sum > 0)
        {
          /* Process input one block at a time.  Note that BLOCKSIZE %
             3 == 0, so that no base64 pads will appear in output. */
          base64_encode (inbuf, sum, outbuf, BASE64_LENGTH (sum));

          wrap_write (outbuf, BASE64_LENGTH (sum), wrap_column,
                      &current_column, out);
        }
    }
  while (!feof (in) && !ferror (in) && sum == BLOCKSIZE);

  /* When wrapping, terminate last line. */
  if (wrap_column && current_column > 0 && fputs ("\n", out) < 0)
    error (EXIT_FAILURE, errno, _("write error"));

  if (ferror (in))
    error (EXIT_FAILURE, errno, _("read error"));
}

