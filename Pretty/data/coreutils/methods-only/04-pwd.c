/* Return a string (malloc'd) consisting of N `/'-separated ".." components.  */
static char *
nth_parent (size_t n)
{
  char *buf = xnmalloc (3, n);
  char *p = buf;
  size_t i;

  for (i = 0; i < n; i++)
    {
      memcpy (p, "../", 3);
      p += 3;
    }
  p[-1] = '\0';
  return buf;
}

