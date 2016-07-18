/* Plain cat.  Copies the file behind `input_desc' to STDOUT_FILENO.
   Return true if successful.  */

static bool
simple_cat (
     /* Pointer to the buffer, used by reads and writes.  */
     char *buf,

     /* Number of characters preferably read or written by each read and write
        call.  */
     size_t bufsize)
{
  /* Actual number of characters read, and therefore written.  */
  size_t n_read;

  /* Loop until the end of the file.  */

  while (true)
    {
      /* Read a block of input.  */

      n_read = safe_read (input_desc, buf, bufsize);
      if (n_read == SAFE_READ_ERROR)
        {
          error (0, errno, "%s", infile);
          return false;
        }

      /* End of this file?  */

      if (n_read == 0)
        return true;

      /* Write this block out.  */

      {
        /* The following is ok, since we know that 0 < n_read.  */
        size_t n = n_read;
        if (full_write (STDOUT_FILENO, buf, n) != n)
          error (EXIT_FAILURE, errno, _("write error"));
      }
    }
}

