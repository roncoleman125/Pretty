/* Copy NREAD bytes of BUF, with no conversions.  */

static void
copy_simple (char const *buf, size_t nread)
{
  const char *start = buf;	/* First uncopied char in BUF.  */

  do
    {
      size_t nfree = MIN (nread, output_blocksize - oc);

      memcpy (obuf + oc, start, nfree);

      nread -= nfree;		/* Update the number of bytes left to copy. */
      start += nfree;
      oc += nfree;
      if (oc >= output_blocksize)
        write_output ();
    }
  while (nread != 0);
}

