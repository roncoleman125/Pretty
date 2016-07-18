/* Return a pointer to a formatted version of F->stat.st_ino,
   possibly using buffer, BUF, of length BUFLEN, which must be at least
   INT_BUFSIZE_BOUND (uintmax_t) bytes.  */
static char *
format_inode (char *buf, size_t buflen, const struct fileinfo *f)
{
  assert (INT_BUFSIZE_BOUND (uintmax_t) <= buflen);
  return (f->stat_ok && f->stat.st_ino != NOT_AN_INODE_NUMBER
          ? umaxtostr (f->stat.st_ino, buf)
          : (char *) "?");
}

