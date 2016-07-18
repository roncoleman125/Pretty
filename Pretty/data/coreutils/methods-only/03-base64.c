static void
do_decode (FILE *in, FILE *out, bool ignore_garbage)
{
  char inbuf[B64BLOCKSIZE];
  char outbuf[BLOCKSIZE];
  size_t sum;
  struct base64_decode_context ctx;

  base64_decode_ctx_init (&ctx);

  do
    {
      bool ok;
      size_t n;
      unsigned int k;

      sum = 0;
      do
        {
          n = fread (inbuf + sum, 1, B64BLOCKSIZE - sum, in);

          if (ignore_garbage)
            {
              size_t i;
              for (i = 0; n > 0 && i < n;)
                if (isbase64 (inbuf[sum + i]) || inbuf[sum + i] == '=')
                  i++;
                else
                  memmove (inbuf + sum + i, inbuf + sum + i + 1, --n - i);
            }

          sum += n;

          if (ferror (in))
            error (EXIT_FAILURE, errno, _("read error"));
        }
      while (sum < B64BLOCKSIZE && !feof (in));

      /* The following "loop" is usually iterated just once.
         However, when it processes the final input buffer, we want
         to iterate it one additional time, but with an indicator
         telling it to flush what is in CTX.  */
      for (k = 0; k < 1 + !!feof (in); k++)
        {
          if (k == 1 && ctx.i == 0)
            break;
          n = BLOCKSIZE;
          ok = base64_decode_ctx (&ctx, inbuf, (k == 0 ? sum : 0), outbuf, &n);

          if (fwrite (outbuf, 1, n, out) < n)
            error (EXIT_FAILURE, errno, _("write error"));

          if (!ok)
            error (EXIT_FAILURE, 0, _("invalid input"));
        }
    }
  while (!feof (in));
}

