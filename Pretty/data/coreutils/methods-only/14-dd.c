/* Write, then empty, the output buffer `obuf'. */

static void
write_output (void)
{
  size_t nwritten = iwrite (STDOUT_FILENO, obuf, output_blocksize);
  w_bytes += nwritten;
  if (nwritten != output_blocksize)
    {
      error (0, errno, _("writing to %s"), quote (output_file));
      if (nwritten != 0)
        w_partial++;
      quit (EXIT_FAILURE);
    }
  else
    w_full++;
  oc = 0;
}

