/* Read at most SIZE bytes from stdin into the array BUF.  Then
   perform the in-place and one-to-one mapping specified by the global
   array `xlate'.  Return the number of characters read, or 0 upon EOF.  */

static size_t
read_and_xlate (char *buf, size_t size)
{
  size_t bytes_read = plain_read (buf, size);
  size_t i;

  for (i = 0; i < bytes_read; i++)
    buf[i] = xlate[to_uchar (buf[i])];

  return bytes_read;
}

