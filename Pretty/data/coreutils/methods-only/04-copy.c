/* Write N_BYTES zero bytes to file descriptor FD.  Return true if successful.
   Upon write failure, set errno and return false.  */
static bool
write_zeros (int fd, uint64_t n_bytes)
{
  static char *zeros;
  static size_t nz = IO_BUFSIZE;

  /* Attempt to use a relatively large calloc'd source buffer for
     efficiency, but if that allocation fails, resort to a smaller
     statically allocated one.  */
  if (zeros == NULL)
    {
      static char fallback[1024];
      zeros = calloc (nz, 1);
      if (zeros == NULL)
        {
          zeros = fallback;
          nz = sizeof fallback;
        }
    }

  while (n_bytes)
    {
      uint64_t n = MIN (nz, n_bytes);
      if ((full_write (fd, zeros, n)) != n)
        return false;
      n_bytes -= n;
    }

  return true;
}

